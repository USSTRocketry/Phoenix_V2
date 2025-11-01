#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

namespace ra
{
class LIS3MDL : public ISensor<SensorData>
{
public:
    bool Init() override;
    bool CollectData(SensorData&) override;

public:
    CtorWrapper(LIS3MDL, m_LIS3);

private:
    SensorMagnetometer m_LIS3;
};
} // namespace ra
