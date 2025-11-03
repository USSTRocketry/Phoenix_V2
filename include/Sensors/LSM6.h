#pragma once

#include <cstdint>
#include "Avionics_HAL.h"
#include "SensorInterface.h"
#include "SensorData.h"

namespace ra
{
class LSM6 final : public ITickedSensor<SensorData>
{
    using ParentClass = ITickedSensor<SensorData>;

public:
    bool Init() override;
    bool Deinit() override { return true; }

public:
    explicit LSM6(ParentClass::TickProvider TP, auto&&... Args) :
        ParentClass(TP), m_LSM6(std::forward<decltype(Args)>(Args)...)
    {
    }

protected:
    bool OnCollectData(SensorData&) override;
    bool History(SensorData&) override;

private:
    SensorAccelGyro m_LSM6;
};
} // namespace ra
