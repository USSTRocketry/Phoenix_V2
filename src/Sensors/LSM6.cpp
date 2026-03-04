#include "Sensors/LSM6.h"

namespace ra
{
bool LSM6::Init() { return m_LSM6.begin() == HAL::SensorStatus::Success; }

bool LSM6::OnCollectData(SensorData& Data)
{
    const HAL::AccelGyroData& D = m_LSM6.read();

    m_History = {
        .Temperature = D.temperature,
        .Accel       = {.X = D.accelX, .Y = D.accelY, .Z = D.accelZ},
        .Gyro        = { .X = D.gyroX,  .Y = D.gyroY,  .Z = D.gyroZ},
    };
    Data.AccelGyro = m_History;

    return true;
}

bool LSM6::History(SensorData& Data)
{
    Data.AccelGyro = m_History;
    return true;
}
} // namespace ra
