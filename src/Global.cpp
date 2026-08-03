#include "Global.h"
#include <PINS.h>
#include <addresses.h>

namespace ra::global
{
WDT_T4<WDT2> WatchDog;

// ==== Wrapped Sensors (internally construct HAL drivers) ====
LIS3MDL Magnetometer(GetSysTick, I2C_ADDRESS_LIS3MDL, HAL::I2C_WIRE);
BMP280 Barometer {GetSysTick, I2C_ADDRESS_BMP280, HAL::I2C_WIRE};
LSM6 AccelGyro(GetSysTick, I2C_ADDRESS_LSM6DS, HAL::I2C_WIRE);

// ==== Standalone HAL Sensors ====
HAL::GpsSensor GpsSensor(HAL::GPS_HW_SERIAL);
HAL::Bmp581Sensor Bmp581(I2C_ADDRESS_BMP581, HAL::I2C_WIRE);
HAL::TemperatureSensor TempSensor(I2C_ADDRESS_MCP9808, HAL::I2C_WIRE);

ra::Logger& Logger = ra::Logger::Instance();
hal::WorkQueue MainQueue;
} // namespace ra::global

namespace ra::global::calibration
{
GroundNormalData GroundNormal {Eigen::Vector3f::Zero(), 0.f};
struct SensorData SensorData {};
} // namespace ra::global::calibration
