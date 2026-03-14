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

namespace hal    = ra::hal;
namespace global = ra::global;

namespace
{
constexpr auto SystemTickId         = 1;
constexpr auto LowPassFactor        = 0.6f;
constexpr int CalibrationIterations = 50;

StateMachine SM;
Filter::LowPass LowPassFilter {LowPassFactor};
ra::SensorAggregator<SensorData> SensorAccumulator {&global::Magnetometer, &global::Barometer, &global::AccelGyro};
hal::Tick SystemTick {SystemTickId};
hal::Tick::TickPoint MainTick {hal::Tick::Invalid()};
hal::WorkQueue::WorkHandle FlightControlHandle {};

ra::Logger::LogInfo Info {
    .Timestamp = MainTick.Raw(), .Level = ra::Logger::Severity::Error, .Location = ra::Logger::Module::Turtleford};

void FlightProcess(hal::WorkQueue::WorkHandle&);

uint32_t WriteBytes(std::span<const std::byte> Data, void*)
{
    const auto LogMsg = ra::turtleford::ProtoDecode_LogMessage(Data);
    if (!LogMsg.has_value())
    {
        Serial.println("log message decode failed");
        return Data.size();
    }

    const Proto_LogMessage DecodedMsg = LogMsg.value();

    switch (DecodedMsg.main_message.which_message_type)
    {
        case Proto_MainMessage_debug_msg_tag:
        {
            const auto Msg = std::unique_ptr<std::string>(
                static_cast<std::string*>(DecodedMsg.main_message.message_type.debug_msg.msg.arg));

            Serial.println(Msg->c_str());
        }
        case Proto_MainMessage_in_flight_data_tag:
        {
            const auto Msg = DecodedMsg.main_message.message_type.in_flight_data;

            const auto PrintVec3 = [](const Proto_InFlightData_VectorF& V)
            { Serial.printf("X %f, Y %f, Z %f\n", V.X, V.Y, V.Z); };

            Serial.printf("timestamp %u,\n", Msg.timestamp_ms);
            Serial.printf("\tBMP : temp %f, pressure %f, alt %f\n",
                          Msg.bmp_data.temperature,
                          Msg.bmp_data.pressure,
                          Msg.bmp_data.altitude);
            Serial.printf("accel gyro temp %f,\n", Msg.accel_gyro_temperature);
            Serial.printf("\tAccel : ");
            PrintVec3(Msg.accel);
            Serial.printf("\tGryo : ");
            PrintVec3(Msg.gyro);
            Serial.printf("\tMag : ");
            PrintVec3(Msg.magnetometer);
            Serial.printf("thermo %f,\n", Msg.thermometer);
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

    for (int i = 0; i < CalibrationIterations; i++)
    {
        auto [Result, Data] = SensorAccumulator.Collect();
        if (!Result)
        {
            ReadingMiss++;
            continue;
        }

        LowPassFilter.Filter(Data);
    }

    if (ReadingMiss > (CalibrationIterations / 2))
    {
        Logger.Log(Info, "Calibration failed");
        assert(false);
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

        .Sched = {.DelayMs = 200, .Iterations = hal::WorkQueue::Scheduling::IterationInfinite}
    };

    std::tie(Status, FlightControlHandle) = global::MainQueue.Submit(FlightControl);
}

void WatchDogInterrupt()
{
    global::Logger.Log(Info, "Watchdog soft interrupt!");
    if (SM.GetState() < FlightState_InFlight)
    {
        global::Logger.Log(Info, "Watchdog enter InFlight");
        // wait for parachute deployment
        SM.EnterState<InFlight>(LowPassFilter.History().BMP280.Altitude);
    }
}

void FlightProcess(hal::WorkQueue::WorkHandle&)
{
    SystemTick.Advance();
    auto [Result, Data] = SensorAccumulator.Collect();
    if (!Result) { global::Logger.Log(Info, "data collection failed"); }

    auto Filtered = LowPassFilter.Filter(Data);
    const StateContext Ctx {.Sensors = Filtered, .FlightControlHandle = FlightControlHandle};
    // StoreData(SM.Run(Ctx), Filtered);
}
} // namespace

hal::Tick::TickPoint global::GetSysTick() { return MainTick; }

void setup()
{
    Serial.begin(115200);
    // InitDataStorage();

    global::Logger.RegisterCallback(WriteBytes, nullptr);

    InitializeSensors();
    CalibrateSensors();

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    global::WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});

    StartFlightControl();

    global::Logger.Log(Info, "FC Start");
}

void loop()
{
    global::WatchDog.feed();

#if !(WORK_QUEUE_PREEMPTIVE)
    global::MainQueue.Run();
#endif
}
