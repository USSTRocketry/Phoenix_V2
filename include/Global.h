#pragma once

#include <cstddef>

#include "Log.h"
#include "Watchdog_t4.h"
#include "ArduinoEigen.h"

#include "SensorData.h"
#include "Sensors/LSM6.h"
#include "Sensors/BMP280.h"
#include "Sensors/LIS3MDL.h"

#include "Avionics_HAL.h"

namespace ra::global
{
extern WDT_T4<WDT2> WatchDog;

// Sensor list
extern LIS3MDL Magnetometer;
extern BMP280 Barometer;
extern LSM6 AccelGyro;
extern HAL::GpsSensor GpsSensor;
extern HAL::Bmp581Sensor Bmp581;
extern HAL::TemperatureSensor TempSensor;

// Ticks
hal::Tick::TickPoint GetSysTick();

extern ra::Logger& Logger;

// Workqueue
extern hal::WorkQueue MainQueue;

// "ground truth" calibration values
namespace calibration
{
struct GroundNormalData
{
    Eigen::Vector3f Direction;
    float Magnitude;
};
extern GroundNormalData GroundNormal;
extern SensorData SensorData;
} // namespace calibration
} // namespace ra::global
