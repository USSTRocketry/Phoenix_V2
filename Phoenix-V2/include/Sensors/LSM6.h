#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

class LSM6 : public ISensor<SensorData>
{
public:
    bool Init();
    bool CollectData(SensorData&);

public:
    CtorWrapper(LSM6, m_LSM6);

private:
    SensorAccelGyro m_LSM6;
};
