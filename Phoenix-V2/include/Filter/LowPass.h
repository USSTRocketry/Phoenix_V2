#pragma once

#include "IFilter.h"

namespace Filter
{
// Exponential moving averaging
class LowPass : public IFilter<SensorData>
{
public:
    virtual SensorData Filter(const SensorData& NewData) override;

    // percentage of old value to keep
    float SetAlpha(float Alpha) { return m_Alpha = Alpha; }
    SensorData History() const { return m_History; }

public:
    explicit LowPass(float Alpha = 0.5, SensorData History = {}) : m_Alpha(Alpha), m_History(History)
    {
        assert(0 <= m_Alpha && m_Alpha <= 1);
        m_History.AccelGyroData.Accel = {};
        m_History.AccelGyroData.Gyro = {};
        m_History.AccelGyroData.Temperature = {};
        m_History.BMP280.Altitude = {};
        m_History.BMP280.Pressure= {};
        m_History.BMP280.Temperature= {};
        m_History.Magnetic = {};
    }
    virtual ~LowPass() override {}

private:
    // percentage of old value to keep
    float m_Alpha;
    // Historical value
    SensorData m_History;
};
} // namespace Filter
