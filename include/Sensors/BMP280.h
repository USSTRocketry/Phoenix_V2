#pragma once

#include <cstdint>
#include "Avionics_HAL.h"
#include "SensorInterface.h"
#include "SensorData.h"

namespace ra
{
class BMP280 final : public ITickedSensor<SensorData>
{
    using ParentClass = ITickedSensor<SensorData>;

public:
    bool Init() override;
    bool Deinit() override { return true; }

public:
    explicit BMP280(ParentClass::TickProvider TP, auto&&... Args) :
        ParentClass(TP), m_BMP(std::forward<decltype(Args)>(Args)...)
    {
    }

protected:
    bool OnCollectData(SensorData&) override;
    bool History(SensorData&) override;

private:
    SensorBMP280 m_BMP;
};
} // namespace ra
