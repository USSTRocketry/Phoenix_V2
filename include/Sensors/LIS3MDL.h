#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

class LIS3MDL : public ISensor<SensorData>
{
public:
    bool Init();
    bool CollectData(SensorData&);

public:
    CtorWrapper(LIS3MDL, m_LIS3);

private:
    SensorMagnetometer m_LIS3;
};
