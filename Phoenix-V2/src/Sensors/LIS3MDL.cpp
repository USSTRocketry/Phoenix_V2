#include "Sensors/LIS3MDL.h"

bool LIS3MDL::Init()
{
    if (!m_LIS3MDL.begin_I2C(m_I2C_Addr, m_I2C_Wire))
    {
        return false;
    }
    m_LIS3MDL.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    m_LIS3MDL.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    m_LIS3MDL.setDataRate(LIS3MDL_DATARATE_155_HZ);
    m_LIS3MDL.setRange(LIS3MDL_RANGE_4_GAUSS);

    return true;
}

bool LIS3MDL::CollectData(SensorData& Data)
{
    sensors_event_t MagEvent;
    if (!m_LIS3MDL.getEvent(&MagEvent))
    {
        return false;
    }
    Data.Magnetic = {.X = MagEvent.magnetic.x, .Y = MagEvent.magnetic.y, .Z = MagEvent.magnetic.z};

    return true;
}
