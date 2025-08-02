#include "Sensors/LIS3MDL.h"

bool LIS3MDL::Init() { return m_LIS3.begin(); }

bool LIS3MDL::CollectData(SensorData& Data)
{
    MagnetometerData* D = m_LIS3.read();
    if (!D) { return false; }

    // Data.Magnetic.x() = D->magneticX;
    // Data.Magnetic.y() = D->magneticY;
    // Data.Magnetic.z() = D->magneticZ;
    Data.Magnetic = {.X = D->magneticX, .Y = D->magneticY, .Z = D->magneticZ};


    // Serial.println("\nMagneto X: ");
    // Serial.println(Data.Magnetic.x() = D->magneticX);
    // Serial.println("Magneto Y: ");
    // Serial.println(Data.Magnetic.x() = D->magneticY);
    // Serial.println("Magneto Z: ");
    // Serial.println(Data.Magnetic.x() = D->magneticZ);

    return true;
}
