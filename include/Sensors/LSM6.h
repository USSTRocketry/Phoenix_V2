#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

namespace ra
{
class LSM6 : public ISensor<SensorData>
{
public:
    bool Init() override;
    bool CollectData(SensorData&) override;

public:
    CtorWrapper(LSM6, m_LSM6);

private:
    SensorAccelGyro m_LSM6;
};
} // namespace ra
