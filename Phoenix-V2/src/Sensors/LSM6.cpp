#include "Sensors/LSM6.h"

bool LSM6::Init() { return m_LSM6.begin(); }

bool LSM6::CollectData(SensorData& Data)
{
    AccelGyroData* D = m_LSM6.read();
    if (!D) { return false; }

    Data.AccelGyroData.Temperature = D->temperature;

    Data.AccelGyroData.Accel       = {.X = D->accelX, .Y = D->accelY, .Z = D->accelZ};
    Data.AccelGyroData.Gyro        = {.X = D->gyroX, .Y = D->gyroY, .Z = D->gyroZ};

    return true;
}

