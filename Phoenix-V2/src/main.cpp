#include <Arduino.h>
#include "Watchdog_t4.h"

#include "Global.h"
#include "States.h"
#include "StateMachine.h"

#include "SensorAggregator.h"
#include "Sensors/LSM6.h"
#include "Sensors/BMP280.h"
#include "Sensors/LIS3MDL.h"

#include "Filter/LowPass.h"

WDT_T4<WDT2> WatchDog;

StateMachine SM;
SensorAggregator<SensorData> SensorAccum;
Filter::LowPass LowPassFilter {0.65};

// Sensor list
LIS3MDL Magnetometer;
BMP280 Barometer;
LSM6 AccelGyro;

void Execute();

void WatchDogInterrupt()
{
    if (!ra::global::ParachuteDeployed)
    {
        // wait for parachute deployment
        SM.EnterState<InFlight>(LowPassFilter.History().BMP280.Altitude);
    }

    // soft reset
    while (true)
    {
        WatchDog.feed();
        Execute();
    }
}

void setup()
{
    Serial.begin(0);

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});

    // set up all sensors
    Magnetometer.Init();
    Barometer.Init();
    AccelGyro.Init();
    SensorAccum.AddSensor({&Magnetometer, &Barometer, &AccelGyro});

    // calibrate and obtain initial readings
    // make sure the GroundNormal is always pointing up
    auto ReadingMiss {0};
    for (auto i = 0; i < 50; i++)
    {
        WatchDog.feed();

        auto [Result, Data] = SensorAccum.Collect();
        if (!Result)
        {
            ReadingMiss++;
            continue;
        }

        LowPassFilter.Filter(Data);
    }
    if (ReadingMiss > 25) { assert(false); }

    ra::global::calibration::SensorData = LowPassFilter.History();
}

void loop()
{
    WatchDog.feed();
    Execute();
}

void Execute()
{
    auto [Result, Data] = SensorAccum.Collect();
    if (!Result) { Serial.println("data collection failed"); }

    auto Filtered = LowPassFilter.Filter(Data);
    SM.Run(Filtered);

    // ra::global::calibration::SensorData = Filtered;
}
