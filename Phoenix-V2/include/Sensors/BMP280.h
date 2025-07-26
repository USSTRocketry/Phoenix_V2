#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

class BMP280 : public ISensor<SensorData>
{
public:
    bool Init();
    bool CollectData(SensorData&);

public:
    CtorWrapper(BMP280, m_BMP);

private:
    SensorBMP280 m_BMP;
};
