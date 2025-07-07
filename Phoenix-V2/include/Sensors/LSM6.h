#pragma once

#include <cstdint>
#include "Avionics_HAL.h"
#include "Sensor.h"
class LSM6 : public ISensor
{
public:
    LSM6(auto... arg) : m_LSM6(std::forward<decltype(arg)>(arg)...) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    SensorAccelGyro m_LSM6;
};
