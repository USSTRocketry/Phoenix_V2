#pragma once

#include <cstdint>

#include "Avionics_HAL.h"
#include "Sensor.h"

class LIS3MDL : public ISensor
{
public:
    LIS3MDL(auto... arg) : m_LIS3(std::forward<decltype(arg)>(arg)...) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    SensorMagnetometer m_LIS3;
};
