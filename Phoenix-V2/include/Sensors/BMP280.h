#pragma once

#include <cstdint>

#include "Avionics_HAL.h"
#include "Sensor.h"

class BMP280 : public ISensor
{
public:
    BMP280(auto... arg) : m_BMP(std::forward<decltype(arg)>(arg)...) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    SensorBMP280 m_BMP;
};
