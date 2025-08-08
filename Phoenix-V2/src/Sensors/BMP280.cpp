#include "Sensors/BMP280.h"

bool BMP280::Init() { return m_BMP.begin(); }

bool BMP280::CollectData(SensorData& Data)
{
    BMP280Data* D = m_BMP.read();
    if (!D) { return false; }

    Data.BMP280 = {.Temperature = D->temperature, .Pressure = D->pressure, .Altitude = D->altitude};

    return true;
}
