#pragma once

#include <cstddef>

#include "Watchdog_t4.h"
#include "ArduinoEigen.h"

#include "SensorData.h"
#include "Sensors/LSM6.h"
#include "Sensors/BMP280.h"
#include "Sensors/LIS3MDL.h"

namespace ra::global
{
extern WDT_T4<WDT2> WatchDog;
extern bool ParachuteDeployed;

// Sensor list
extern LIS3MDL Magnetometer;
extern BMP280 Barometer;
extern LSM6 AccelGyro;

// Ticks
hal::Tick::TickPoint GetSysTick();

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
