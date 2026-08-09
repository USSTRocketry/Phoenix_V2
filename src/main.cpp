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
#include <Wire.h>
#include "Type.h"

namespace hal    = ra::hal;
namespace global = ra::global;

namespace
{
HAL::RFM95Radio Radio(HAL::RADIO_CS, HAL::RADIO_INT, HAL::RADIO_SPI, 433.0);

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

bool g_camera_power = false;
bool g_camera_recording = false;

ra::Logger::LogInfo DefaultLogInfo{
    .Timestamp = ra::hal::SysUptimeMs(),
    .Level     = ra::Logger::Severity::Info,
    .Category  = ra::type::Category::Sensors,
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

    std::span<const std::byte> remaining = Data;
    while (!remaining.empty())
    {
        auto FrameOpt = ra::turtleford::ProtoFrame_Read(remaining);
        if (!FrameOpt.has_value())
        {
            Serial.println("log message frame decode failed");
            break;
        }

        const auto& Frame = FrameOpt.value();
        const auto LogMsgOpt = ra::turtleford::ProtoDecode_LogMessage(Frame.Payload, ra::turtleford::ProtoFlags::None);
        if (!LogMsgOpt.has_value())
        {
            Serial.println("log message payload decode failed");
            remaining = remaining.subspan(Frame.BytesConsumed);
            continue;
        }

        const Proto_LogMessage& DecodedMsg = LogMsgOpt.value();

        // Serial.printf("Timestamp : %f\n", static_cast<double>(DecodedMsg.main_message.timestamp));

        switch (DecodedMsg.main_message.which_message_type)
        {
            case Proto_MainMessage_debug_msg_tag:
            {
                const auto Msg = std::unique_ptr<std::string>(
                    static_cast<std::string*>(DecodedMsg.main_message.message_type.debug_msg.msg.arg));

                // Serial.println(Msg->c_str());
                break;
            }
            case Proto_MainMessage_in_flight_data_tag:
            {
                // Re-serialize as raw Proto_MainMessage (without the LogMessage outer wrapper) and send over RFM95
                static uint32_t LastRadioSend = 0;
                uint32_t Now = millis();
                if (Now - LastRadioSend >= 400) // 400ms throttle to prevent clogging the radio
                {
                    LastRadioSend = Now;
                    static std::array<std::byte, 128> radioBuffer;
                    size_t written = ra::turtleford::ProtoEncode(
                        DecodedMsg.main_message.timestamp,
                        DecodedMsg.main_message,
                        radioBuffer,
                        ra::turtleford::ProtoFlags::None
                    );
                    if (written > 0)
                    {
                        bool sent = Radio.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(radioBuffer.data()), written));
                        // Serial.printf("Radio.send called! Bytes: %d, success: %d\n", (int)written, sent);
                        (void)sent;
                    }
                }
                break;
            }
        }

        remaining = remaining.subspan(Frame.BytesConsumed);
    }

    return Data.size();
}

void InitializeSensors()
{
    SensorAccumulator.Apply([](auto* Sensor) { Sensor->Init(); });
    global::GpsSensor.begin();
    global::Bmp581.begin();
    global::TempSensor.begin();
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

    const float AccelMag = calibration::SensorData.AccelGyro.Accel.norm();
    if (AccelMag > 1.0f)
    {
        calibration::GroundNormal = {calibration::SensorData.AccelGyro.Accel / AccelMag, AccelMag};
    }
    else
    {
        calibration::GroundNormal = {Eigen::Vector3f(0.f, 0.f, 1.f), 9.81f};
    }
}

void StartFlightControl()
{
    // Serial.println("Initializing MainQueue...");
    auto Status = global::MainQueue.Init();
    // Serial.printf("MainQueue Init status: %d\n", static_cast<int>(Status));

    hal::WorkQueue::SubmitOptions FlightControl {
        .Exec =
            {
                   .Fn  = FlightProcess,
                   .Ctx = nullptr,
                   },

        .Sched = {.DelayMs = TickFrequencyMs, .Iterations = hal::WorkQueue::Scheduling::IterationInfinite}
    };

    // Serial.println("Submitting FlightProcess to MainQueue...");
    std::tie(Status, FlightControlHandle) = global::MainQueue.Submit(FlightControl);
    // Serial.printf("MainQueue Submit status: %d\n", static_cast<int>(Status));
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

static ra::type::FlightState ToFlightState(FlightState State)
{
    switch (State)
    {
        case FlightState_Unarmed: return ra::type::FlightState::Unarmed;
        case FlightState_GroundIdle: return ra::type::FlightState::GroundIdle;
        case FlightState_InFlight: return ra::type::FlightState::InFlight;
        case FlightState_MainChute: return ra::type::FlightState::MainChute;
        default: return ra::type::FlightState::Unknown;
    }
}

static ra::type::FlightData SensorDataToFlightData(const SensorData& SensorData, FlightState State)
{
    const auto GpsData = global::GpsSensor.read();

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
        .State                = ToFlightState(State),
        .GPS_Data             = {.Latitude   = GpsData.latitude,
                                 .Longitude  = GpsData.longitude,
                                 .Altitude   = GpsData.altitude,
                                 .Speed      = GpsData.speed,
                                 .Angle      = GpsData.angle,
                                 .FixQuality = static_cast<uint32_t>(GpsData.fix_quality),
                                 .Satellites  = static_cast<uint32_t>(GpsData.satellites)},
    };
}

void FlightProcess(hal::WorkQueue::WorkHandle&)
{
    // Serial.println("--- FlightProcess Tick Start ---");
    MainTick            = SystemTick.Advance();
    // Serial.println("Tick advanced");
    auto [Result, Data] = SensorAccumulator.Collect();
    // Serial.printf("Sensors collected, Result: %d\n", Result);
    if (!Result)
    {
        // Serial.println("Warning: Sensor collection failed!");
        LogApp(1, "data collection failed", ra::Logger::Severity::Warn);
    }

    // Serial.println("Filtering data...");
    auto Filtered = LowPassFilter.Filter(Data);
    // Serial.println("Data filtered");

    const StateContext Ctx {.Sensors = Filtered, .FlightControlHandle = FlightControlHandle};
    // Serial.println("Running StateMachine...");
    SM.Run(Ctx);
    // Serial.println("StateMachine run complete");

    const ra::type::FlightData Fd = SensorDataToFlightData(Filtered, SM.GetState());
    ra::Logger::LogInfo dataInfo = DefaultLogInfo;
    dataInfo.Timestamp = ra::hal::SysUptimeMs();
    dataInfo.Level     = ra::Logger::Severity::Verbose;
    dataInfo.Category  = ra::type::Category::Sensors;
    // Serial.println("Logging flight data...");
    ra::global::Logger.Log(dataInfo, Fd);
    // Serial.println("Flight data logged");

    // Force flush the cached buffer to output the logged data immediately
    // Serial.println("Flushing logger...");
    ra::global::Logger.Flush();
    // Serial.println("Logger flushed");
    // Serial.println("--- FlightProcess Tick End ---");
}

void HandleCommand(uint32_t cmd)
{
    switch (cmd)
    {
        case static_cast<uint32_t>(ra::type::CommandType::Abort):
            // Serial.println("[COMMAND RECEIVED] ABORT: Command received, no action taken.");
            LogApp(0, "Command: Abort received (no-op)", ra::Logger::Severity::Info);
            break;
        case static_cast<uint32_t>(ra::type::CommandType::CameraOn):
            g_camera_power = true;
            // Serial.println("[COMMAND RECEIVED] CAMERA ON: Power enabled.");
            LogApp(0, "Command: Camera ON", ra::Logger::Severity::Info);
            break;
        case static_cast<uint32_t>(ra::type::CommandType::CameraOff):
            g_camera_power = false;
            g_camera_recording = false;
            // Serial.println("[COMMAND RECEIVED] CAMERA OFF: Power disabled.");
            LogApp(0, "Command: Camera OFF", ra::Logger::Severity::Info);
            break;
        case static_cast<uint32_t>(ra::type::CommandType::StartRecording):
            if (g_camera_power) {
                g_camera_recording = true;
                // Serial.println("[COMMAND RECEIVED] START RECORDING: Video recording started.");
                LogApp(0, "Command: Start Recording", ra::Logger::Severity::Info);
            } else {
                // Serial.println("[COMMAND WARNING] START RECORDING: Camera is OFF. Power on camera first!");
                LogApp(1, "Command: Start Recording failed (Camera OFF)", ra::Logger::Severity::Warn);
            }
            break;
        case static_cast<uint32_t>(ra::type::CommandType::StopRecording):
            g_camera_recording = false;
            // Serial.println("[COMMAND RECEIVED] STOP RECORDING: Video recording stopped.");
            LogApp(0, "Command: Stop Recording", ra::Logger::Severity::Info);
            break;
        case static_cast<uint32_t>(ra::type::CommandType::SdLogOn):
            SetSdLoggingEnabled(true);
            LogApp(0, "Command: SD Logging ON", ra::Logger::Severity::Info);
            break;
        case static_cast<uint32_t>(ra::type::CommandType::SdLogOff):
            SetSdLoggingEnabled(false);
            LogApp(0, "Command: SD Logging OFF", ra::Logger::Severity::Info);
            break;
        default:
            // Serial.printf("[COMMAND ERROR] Unknown command ID: %u\n", cmd);
            LogApp(1, "Unknown command received", ra::Logger::Severity::Warn);
            break;
    }
}

void ProcessIncomingRadioCommands()
{
    static std::array<std::byte, 128> rxBuf;
    size_t len = 0;
    if (Radio.receive(std::span<uint8_t>(reinterpret_cast<uint8_t*>(rxBuf.data()), rxBuf.size()), len))
    {
        // Serial.printf("Radio frame received! Size: %d bytes\n", (int)len);
        std::span<const std::byte> dataSpan(rxBuf.data(), len);
        auto mainMsgOpt = ra::turtleford::ProtoDecode_MainMessage(dataSpan);
        if (mainMsgOpt.has_value())
        {
            const auto& msg = mainMsgOpt.value();
            if (msg.which_message_type == Proto_MainMessage_command_msg_tag)
            {
                uint32_t cmd = msg.message_type.command_msg.command;
                HandleCommand(cmd);
            }
        }
    }
}
} // namespace

hal::Tick::TickPoint global::GetSysTick() { return SystemTick.Now(); }

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 4000)
    {
        // Wait up to 4 seconds for USB Serial Monitor to connect
    }
    Serial.println("--- Flight Computer Booting ---");
    if (CrashReport)
    {
        Serial.print(CrashReport);
    }

    pinMode(HAL::GPIO_3V3_EN, OUTPUT);
    digitalWrite(HAL::GPIO_3V3_EN, LOW);  // Active LOW according to pms.h
    delay(100); // Allow sensor power rail to stabilize

    Wire.begin();

    Serial.println("Initializing Data Storage...");
    InitDataStorage();
    Serial.println();

    Serial.println("Registering Logger Callback...");
    global::Logger.RegisterCallback(WriteBytes, nullptr);

    Serial.println("Initializing RFM95 Radio...");
    // Initialize radio EN and reset
    pinMode(HAL::RADIO_EN, OUTPUT);
    digitalWrite(HAL::RADIO_EN, HIGH);
    Radio.reset(HAL::RADIO_RST);
    if (!Radio.begin())
    {
        Serial.println("RFM95 Radio init failed!");
        LogApp(1, "RFM95 Radio init failed", ra::Logger::Severity::Error);
    }
    else
    {
        Serial.println("RFM95 Radio init OK.");
        LogApp(0, "RFM95 Radio init OK", ra::Logger::Severity::Info);
    }

    Serial.println("Initializing Sensors...");
    InitializeSensors();

    Serial.println("Calibrating Sensors (takes 10s)...");
    CalibrateSensors();
    Serial.println("Calibration complete.");

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    Serial.println("Starting Watchdog...");
    global::WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});

    Serial.println("Starting Flight Control Queue...");
    StartFlightControl();

    LogApp(0, "FC Start", ra::Logger::Severity::Info);
    Serial.println("--- Setup Complete, Entering Loop ---");
}

void loop()
{
    global::WatchDog.feed();

    global::GpsSensor.update();

    ProcessIncomingRadioCommands();

    // static uint32_t LastLoopPrint = 0;
    // uint32_t Now = millis();
    // if (Now - LastLoopPrint >= 1000)
    // {
    //     LastLoopPrint = Now;
    //     Serial.println("Loop tick!");
    // }

#if !(WORK_QUEUE_PREEMPTIVE)
    global::MainQueue.Run();
#endif
}
