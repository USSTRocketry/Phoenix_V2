#include "Sensors/LIS3MDL.h"

bool LIS3MDL::Init() { return m_LIS3.begin(); }

bool LIS3MDL::CollectData(SensorData& Data)
{
    MagnetometerData* D = m_LIS3.read();
    if (!D) { return false; }

    Data.Magnetic.x() = D->magneticX;
    Data.Magnetic.y() = D->magneticY;
    Data.Magnetic.z() = D->magneticZ;
    return true;
}
