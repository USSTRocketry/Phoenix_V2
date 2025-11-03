#include <Arduino.h>

#include "Watchdog_t4.h"
#include "Global.h"
#include "StateMachine.h"
#include "SensorAggregator.h"
#include "Filter/LowPass.h"
#include "Log/DataStorage.h"

StateMachine SM;
Filter::LowPass LowPassFilter {0.6};
ra::SensorAggregator<SensorData> SensorAccumulator {
    &ra::global::Magnetometer, &ra::global::Barometer, &ra::global::AccelGyro};

static constexpr auto SystemTickId = 1;
ra::hal::Tick SystemTick {SystemTickId};
static ra::hal::Tick::TickPoint MainTick {ra::hal::Tick::Invalid()};
ra::hal::Tick::TickPoint ra::global::GetSysTick() { return MainTick; }

static void Run();

void WatchDogInterrupt()
{
    StoreStringLine("Watchdog soft interrupt!");
    if (!ra::global::ParachuteDeployed)
    {
        StoreStringLine("Watchdog enter InFlight");
        // wait for parachute deployment
        SM.EnterState<InFlight>(LowPassFilter.History().BMP280.Altitude);
    }
}

void setup()
{
    using namespace ra::global;

    Serial.begin(115200);
    InitDataStorage();

    // initialize all sensors
    SensorAccumulator.Apply([](auto* Sensor) { Sensor->Init(); });

    // calibrate and obtain initial readings
    // make sure the GroundNormal is always pointing up
    {
        auto ReadingMiss {0};
        constexpr int CalibrateIteration = 50;

        for (auto i = 0; i < CalibrateIteration; i++)
        {
            auto [Result, Data] = SensorAccumulator.Collect();
            if (!Result)
            {
                ReadingMiss++;
                continue;
            }

            LowPassFilter.Filter(Data);
        }
        if (ReadingMiss > CalibrateIteration / 2)
        {
            StoreStringLine("Calibration failed");
            assert(false);
        }

        calibration::SensorData   = LowPassFilter.History();
        const float AccelMag      = calibration::SensorData.AccelGyroData.Accel.norm();
        calibration::GroundNormal = {calibration::SensorData.AccelGyroData.Accel / AccelMag, AccelMag};
    }
    StoreStringLine("FC Start");

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    ra::global::WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});
}

void loop() { Run(); }

void Run()
{
    SystemTick.Advance();
    ra::global::WatchDog.feed();

    auto [Result, Data] = SensorAccumulator.Collect();
    if (!Result) { StoreStringLine("data collection failed"); }

    auto Filtered = LowPassFilter.Filter(Data);
    StoreData(SM.Run(Filtered), Filtered);
}
