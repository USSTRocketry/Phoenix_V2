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

#include "SDHandler.h"

WDT_T4<WDT2> WatchDog;

StateMachine SM;
SensorAggregator<SensorData> SensorAccumulator;
Filter::LowPass LowPassFilter {0.4};

// Sensor list
LIS3MDL Magnetometer(0x1E, I2C_WIRE0);
BMP280 Barometer;
LSM6 AccelGyro(0x6B, I2C_WIRE0);

void Execute();

void WatchDogInterrupt()
{
    StoreStringLineToCSV("Watchdog soft interrupt!");
    if (!ra::global::ParachuteDeployed)
    {
        StoreStringLineToCSV("Watchdog enter InFlight");
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
    Serial.begin(115200);
    delay(3000);
    Serial.println("Setup");
    pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, LOW);

    // for(int i=0; i<10; i++) {
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     delay(1000);
    //     digitalWrite(LED_BUILTIN, LOW);
    //     delay(1000);
    // }

    // soft reset(sec), hard reset(sec), pin, fn_ptr for soft reset
    WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});

    // set up all sensors
    {
        bool SensorInitStatus = Magnetometer.Init();
        SensorInitStatus &= Barometer.Init();
        SensorInitStatus &= AccelGyro.Init();
        SensorAccumulator.AddSensor({&Magnetometer, &Barometer, &AccelGyro});
    }

    // calibrate and obtain initial readings
    // make sure the GroundNormal is always pointing up
    {
        auto ReadingMiss {0};
        constexpr int CalibrateIteration = 50;

        for (auto i = 0; i < CalibrateIteration; i++)
        {
            WatchDog.feed();

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
            StoreStringLineToCSV("Calibration failed");
            assert(false);
        }

        ra::global::calibration::SensorData   = LowPassFilter.History();
        float AccelMag                        = ra::global::calibration::SensorData.AccelGyroData.Accel.norm();
        ra::global::calibration::GroundNormal = {ra::global::calibration::SensorData.AccelGyroData.Accel / AccelMag,
                                                 AccelMag};
    }

    StoreStringLineToCSV("FC Start");
    Serial.println("FC Start 2");
}

void loop()
{
    WatchDog.feed();
    Execute();
    delay(1000);
}

void Execute()
{
    auto [Result, Data] = SensorAccumulator.Collect();
    if (!Result) { StoreStringLineToCSV("data collection failed"); }

    auto Filtered = LowPassFilter.Filter(Data);
    SM.Run(Filtered);
}
