#include "Sensors/LSM6.h"

bool LSM6::Init()
{
    if (!m_LSM6.begin_I2C(m_I2C_Addr, m_I2C_Wire))
    {
        return false;
    }
    return true;
}

bool LSM6::CollectData(SensorData& Data)
{
    sensors_event_t Accel, Gyro, Temp;
    if (!m_LSM6.getEvent(&Accel, &Gyro, &Temp))
    {
        return false;
    }

    Data.AccelGyroData.Temperature = Temp.temperature;
    Data.AccelGyroData.Accel       = {.X = Accel.acceleration.x, .Y = Accel.acceleration.y, .Z = Accel.acceleration.z};
    Data.AccelGyroData.Gyro        = {.X = Gyro.gyro.x, .Y = Gyro.gyro.y, .Z = Gyro.gyro.z};

    return true;
}
