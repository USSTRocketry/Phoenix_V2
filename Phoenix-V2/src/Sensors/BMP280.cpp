#include "Sensors/BMP280.h"

bool BMP280::Init()
{
    if (!m_BMP.begin(m_I2C_Addr))
    {
        return false;
    }

    /* Default settings from datasheet. */
    m_BMP.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                      Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                      Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                      Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                      Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    return true;
}
bool BMP280::CollectData(SensorData& Data)
{
    Data.BMP280 = {.Temperature = m_BMP.readTemperature(),
                   .Pressure    = m_BMP.readPressure(),
                   .Altitude    = m_BMP.readAltitude(m_GroundAltitude)};

    return true;
}
