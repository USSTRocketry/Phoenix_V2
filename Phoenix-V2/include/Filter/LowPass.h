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
    LowPass(float Alpha = 0.5, SensorData History = {}) : m_Alpha(Alpha), m_History(History)
    {
        assert(0 <= m_Alpha && m_Alpha <= 1);
    }
    virtual ~LowPass() override {}

private:
    // percentage of old value to keep
    float m_Alpha;
    // Historical value
    SensorData m_History;
};
} // namespace Filter
