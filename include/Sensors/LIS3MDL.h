#pragma once

#include <cstdint>
#include "Avionics_HAL.h"
#include "SensorInterface.h"
#include "SensorData.h"

namespace ra
{
class LIS3MDL final : public ITickedSensor<SensorData>
{
    using ParentClass = ITickedSensor<SensorData>;
    SensorData::MagnetometerData m_History;

public:
    bool Init() override;
    bool Deinit() override { return true; }

public:
    explicit LIS3MDL(ParentClass::TickProvider TP, auto&&... Args) :
        ParentClass(TP), m_LIS3(std::forward<decltype(Args)>(Args)...)
    {
    }

protected:
    bool OnCollectData(SensorData&) override;
    bool History(SensorData&) override;

private:
    HAL::MagnetometerSensor m_LIS3;
};
} // namespace ra
