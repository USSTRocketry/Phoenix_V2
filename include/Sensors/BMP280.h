#pragma once

#include <cstdint>
#include "SensorDef.h"
#include "SensorInterface.h"

namespace ra
{
class BMP280 : public ISensor<SensorData>
{
public:
    bool Init() override;
    bool CollectData(SensorData&) override;

public:
    CtorWrapper(BMP280, m_BMP);

private:
    SensorBMP280 m_BMP;
};
} // namespace ra
