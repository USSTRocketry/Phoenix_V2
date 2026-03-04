#include "Global.h"
#include <PINS.h>

namespace ra::global
{
WDT_T4<WDT2> WatchDog;

// ==== Global controls ====
bool ParachuteDeployed {false};

// ==== Sensors ====
LIS3MDL Magnetometer(GetSysTick, 0x1E, HAL::I2C_WIRE);
BMP280 Barometer {GetSysTick, 0, HAL::I2C_WIRE};
LSM6 AccelGyro(GetSysTick, 0x6B, HAL::I2C_WIRE);
} // namespace ra::global

namespace ra::global::calibration
{
GroundNormalData GroundNormal {Eigen::Vector3f::Zero(), 0.f};
struct SensorData SensorData {};
} // namespace ra::global::calibration
