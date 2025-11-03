#include "Sensors/BMP280.h"

namespace ra
{
bool BMP280::Init() { return m_BMP.begin(); }

bool BMP280::OnCollectData(SensorData& Data)
{
    BMP280Data* D = m_BMP.read();
    if (!D) { return false; }

    Data.BMP280 = {.Temperature = D->temperature, .Pressure = D->pressure, .Altitude = D->altitude};

    return true;
}
bool BMP280::History(SensorData& Data)
{
    auto& D = m_BMP.data;

    Data.BMP280 = {.Temperature = D.temperature, .Pressure = D.pressure, .Altitude = D.altitude};

    return true;
}
} // namespace ra
