#include <Arduino.h>

#include "RocketGroundCommunication.pb.h"
#include "Watchdog_t4.h"
#include "Global.h"
#include "StateMachine.h"
#include "SensorAggregator.h"
#include "Filter/LowPass.h"
#include "Log/DataStorage.h"
#include "Log.h"
#include "WorkQueue.h"
#include "ProtoCodec.h"
#include <memory>
#include <string>
#include "Type.h"

namespace hal    = ra::hal;
namespace global = ra::global;

namespace
{
constexpr auto SystemTickId          = 1;
constexpr auto LowPassFactor         = 0.6f;
constexpr auto CalibrationIterations = 50;
constexpr auto TickFrequencyMs       = 200;

StateMachine SM;
Filter::LowPass LowPassFilter {LowPassFactor};
ra::SensorAggregator<SensorData> SensorAccumulator {&global::Magnetometer, &global::Barometer, &global::AccelGyro};
hal::Tick SystemTick {SystemTickId};
hal::Tick::TickPoint MainTick {hal::Tick::Invalid()};
hal::WorkQueue::WorkHandle FlightControlHandle {};

ra::Logger::LogInfo DefaultLogInfo {
    .Timestamp = MainTick.Raw(),
    .Level     = ra::Logger::Severity::Error,
    .Category  = ra::type::Category::Platform,
};

void LogApp(uint32_t Status,
            const std::string& Msg,
            ra::Logger::Severity Level = ra::Logger::Severity::Info,
            ra::type::Category Category = ra::type::Category::Application)
{
    auto Info = DefaultLogInfo;
    Info.Timestamp = SystemTick.Now().Raw();
    Info.Level     = Level;
    Info.Category  = Category;
    ra::global::Logger.Log(Info, Status, Msg);
}

void FlightProcess(hal::WorkQueue::WorkHandle&);

uint32_t WriteBytes(std::span<const std::byte> Data, void*)
{
    StoreBytes(const_cast<char*>(reinterpret_cast<const char*>(Data.data())), Data.size_bytes());

    const auto LogMsg = ra::turtleford::ProtoDecode_LogMessage(Data);
    if (!LogMsg.has_value())
    {
        Serial.println("log message decode failed");
        return Data.size();
    }

    const Proto_LogMessage DecodedMsg = LogMsg.value();

    Serial.printf("Timestamp : %f\n", static_cast<double>(DecodedMsg.main_message.timestamp));

    switch (DecodedMsg.main_message.which_message_type)
    {
        case Proto_MainMessage_debug_msg_tag:
        {
            const auto Msg = std::unique_ptr<std::string>(
                static_cast<std::string*>(DecodedMsg.main_message.message_type.debug_msg.msg.arg));

            Serial.println(Msg->c_str());
            break;
        }
        case Proto_MainMessage_in_flight_data_tag:
        {
            const auto Msg = DecodedMsg.main_message.message_type.in_flight_data;

            const auto PrintVec3 = [](const Proto_InFlightData_VectorF& V)
            { Serial.printf("X %f, Y %f, Z %f\n", static_cast<double>(V.X), static_cast<double>(V.Y), static_cast<double>(V.Z)); };

            Serial.printf("timestamp %u,\n", MainTick.Raw());
            Serial.printf("\tBMP : temp %f, pressure %f, alt %f\n",
                          static_cast<double>(Msg.bmp_data.temperature),
                          static_cast<double>(Msg.bmp_data.pressure),
                          static_cast<double>(Msg.bmp_data.altitude));
            Serial.printf("accel gyro temp %f,\n", static_cast<double>(Msg.accel_gyro_temperature));
            Serial.printf("\tAccel : ");
            PrintVec3(Msg.accel);
            Serial.printf("\tGryo : ");
            PrintVec3(Msg.gyro);
            Serial.printf("\tMag : ");
            PrintVec3(Msg.magnetometer);
            Serial.printf("thermo %f,\n", static_cast<double>(Msg.thermometer));
            break;
        }
    }

    return Data.size();
}

void InitializeSensors()
{
    SensorAccumulator.Apply([](auto* Sensor) { Sensor->Init(); });
}

void CalibrateSensors()
{
    using namespace global;

    auto ReadingMiss {0};

    // we do a blocking init
    for (auto i = 0; i < CalibrationIterations; i++)
    {
        // Ensure the tick advances each iteration so ticked sensors will collect fresh data.
        MainTick = SystemTick.Advance();

        auto [Result, Data] = SensorAccumulator.Collect();
        if (!Result)
        {
            ReadingMiss++;
            continue;
        }

        LowPassFilter.Filter(Data);
        delay(TickFrequencyMs);
    }

    if (ReadingMiss > (CalibrationIterations / 2))
    {
        LogApp(1, "Calibration failed", ra::Logger::Severity::Error, ra::type::Category::Sensors);
    }

    calibration::SensorData = LowPassFilter.History();

    const float AccelMag      = calibration::SensorData.AccelGyro.Accel.norm();
    calibration::GroundNormal = {calibration::SensorData.AccelGyro.Accel / AccelMag, AccelMag};
}

void StartFlightControl()
{
    auto Status = global::MainQueue.Init();
    hal::WorkQueue::SubmitOptions FlightControl {
        .Exec =
            {
                   .Fn  = FlightProcess,
                   .Ctx = nullptr,
                   },

        .Sched = {.DelayMs = TickFrequencyMs, .Iterations = hal::WorkQueue::Scheduling::IterationInfinite}
    };

    std::tie(Status, FlightControlHandle) = global::MainQueue.Submit(FlightControl);
}

void WatchDogInterrupt()
{
    LogApp(0, "Watchdog soft interrupt!", ra::Logger::Severity::Warn, ra::type::Category::Platform);
    if (SM.GetState() < FlightState_InFlight)
    {
        LogApp(0, "Watchdog enter InFlight", ra::Logger::Severity::Info, ra::type::Category::FlightControl);
        // wait for parachute deployment
        SM.EnterState<InFlight>(LowPassFilter.History().BMP280.Altitude);
    }
}

static ra::type::FlightData SensorDataToFlightData(const SensorData& SensorData)
{
    return ra::type::FlightData{
        .BMP_Data             = {.Temperature = SensorData.BMP280.Temperature,
                                 .Pressure    = SensorData.BMP280.Pressure,
                                 .Altitude    = SensorData.BMP280.Altitude},
        .AccelGyroTemperature = SensorData.AccelGyro.Temperature,
        .Accel                = {.X = SensorData.AccelGyro.Accel.x(),
                                 .Y = SensorData.AccelGyro.Accel.y(),
                                 .Z = SensorData.AccelGyro.Accel.z()},
        .Gyro                 = {.X = SensorData.AccelGyro.Gyro.x(),
                                 .Y = SensorData.AccelGyro.Gyro.y(),
                                 .Z = SensorData.AccelGyro.Gyro.z()},
        .Magnetometer         = {.X = SensorData.Magnetic.x(),
                                 .Y = SensorData.Magnetic.y(),
                                 .Z = SensorData.Magnetic.z()},
        .Thermometer          = SensorData.BMP280.Temperature,
    };
}

void FlightProcess(hal::WorkQueue::WorkHandle&)
{
    MainTick            = SystemTick.Advance();
    auto [Result, Data] = SensorAccumulator.Collect();
    if (!Result)
    {
        LogApp(1, "data collection failed", ra::Logger::Severity::Warn);
    }

    auto Filtered = LowPassFilter.Filter(Data);
    const StateContext Ctx {.Sensors = Filtered, .FlightControlHandle = FlightControlHandle};
    SM.Run(Ctx);

    const ra::type::FlightData Fd = SensorDataToFlightData(Filtered);
    ra::Logger::LogInfo dataInfo = DefaultLogInfo;
    dataInfo.Timestamp = MainTick.Raw();
    dataInfo.Level     = ra::Logger::Severity::Verbose;
    dataInfo.Category  = ra::type::Category::Sensors;
    ra::global::Logger.Log(dataInfo, Fd);
}
} // namespace

hal::Tick::TickPoint global::GetSysTick() { return SystemTick.Now(); }

void setup()
{
    Serial.begin(115200);
    InitDataStorage();

    global::Logger.RegisterCallback(WriteBytes, nullptr);

    InitializeSensors();
    CalibrateSensors();

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    global::WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});

    StartFlightControl();

    LogApp(0, "FC Start", ra::Logger::Severity::Info);
}

void loop()
{
    global::WatchDog.feed();

#if !(WORK_QUEUE_PREEMPTIVE)
    global::MainQueue.Run();
#endif
}
