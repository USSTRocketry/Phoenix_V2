#include "Sensors/BMP280.h"

namespace ra
{
bool BMP280::Init() { return m_BMP.begin() == HAL::SensorStatus::Success; }

bool BMP280::OnCollectData(SensorData& Data)
{
    const HAL::BMP280Data& D = m_BMP.read();

    m_History   = {.Temperature = D.temperature, .Pressure = D.pressure, .Altitude = D.altitude};
    Data.BMP280 = m_History;

    return true;
}
bool BMP280::History(SensorData& Data)
{
    Data.BMP280 = m_History;
    return true;
}
} // namespace ra
