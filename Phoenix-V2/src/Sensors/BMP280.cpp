#include "Sensors/BMP280.h"

bool BMP280::Init() { return m_BMP.begin(); }

bool BMP280::CollectData(SensorData& Data)
{
    BMP280Data* D = m_BMP.read();
    if (!D) { return false; }

    Data.BMP280 = {.Temperature = D->temperature, .Pressure = D->pressure, .Altitude = D->altitude};
    
    // Serial.println("\nBMP280 Temp: ");
    // Serial.println(Data.BMP280.Temperature);
    // Serial.println("BMP280 Pressure: ");
    // Serial.println(Data.BMP280.Pressure);
    // Serial.println("BMP280 Altitude: ");
    // Serial.println(Data.BMP280.Altitude);

    return true;
}
