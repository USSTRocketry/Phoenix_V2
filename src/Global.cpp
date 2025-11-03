#include "Global.h"

namespace ra::global
{
WDT_T4<WDT2> WatchDog;

// ==== Global controls ====
bool ParachuteDeployed {false};

// ==== Sensors ====
LIS3MDL Magnetometer(GetSysTick, 0x1E, I2C_WIRE0);
BMP280 Barometer {GetSysTick};
LSM6 AccelGyro(GetSysTick, 0x6B, I2C_WIRE0);
} // namespace ra::global

namespace ra::global::calibration
{
GroundNormalData GroundNormal {Eigen::Vector3f::Zero(), 0.f};
struct SensorData SensorData {};
} // namespace ra::global::calibration
