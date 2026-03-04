#include "Sensors/LIS3MDL.h"

namespace ra
{
bool LIS3MDL::Init() { return m_LIS3.begin() == HAL::SensorStatus::Success; }

bool LIS3MDL::OnCollectData(SensorData& Data)
{
    const HAL::MagnetometerData& D = m_LIS3.read();

    m_History     = {.X = D.magneticX, .Y = D.magneticY, .Z = D.magneticZ};
    Data.Magnetic = m_History;

    return true;
}

bool LIS3MDL::History(SensorData& Data)
{
    Data.Magnetic = m_History;
    return true;
}
} // namespace ra
