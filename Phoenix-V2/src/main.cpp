#include <Arduino.h>
#include "Watchdog_t4.h"

#include "Global.h"
#include "StateMachine.h"
#include "SensorAggregator.h"
#include "Sensors/LSM6.h"
#include "Sensors/BMP280.h"
#include "Sensors/LIS3MDL.h"
#include "States.h"

StateMachine SM;
SensorAggregator SensorAccum;
WDT_T4<WDT2> WatchDog;

LIS3MDL Magnetometer;
BMP280 Barometer;
LSM6 AccelGyro;

void WatchDogInterrupt()
{
    // soft reset
    while (true)
    {
        WatchDog.feed();
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

        auto Result = SensorAccum.Collect().first;
        if (!Result) { ReadingMiss++; }
    }
    if (ReadingMiss > 25) { assert(false); }

    ra::global::calibration::SensorData = SensorAccum.History();
}

void loop()
{
    WatchDog.feed();

    auto [Result, Data] = SensorAccum.Collect();
    if (!Result) { Serial.println("data collection failed"); }

    SM.Run(Data);
    ra::global::calibration::SensorData = SensorAccum.History();
}
