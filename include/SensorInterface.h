#pragma once

#include <cassert>
#include "Avionics_HAL.h"

namespace ra
{
template <typename SensorDataType>
class ISensor
{
public:
    ISensor() {}
    virtual ~ISensor() = default;

public:
    virtual bool Init()                            = 0;
    virtual bool CollectData(SensorDataType& Data) = 0;
    virtual bool Deinit()                          = 0;

protected:
    virtual bool OnCollectData(SensorDataType&) = 0;
};

template <typename SensorDataType>
class ITickedSensor : public ISensor<SensorDataType>
{
    using TickPoint = hal::Tick::TickPoint;

public:
    // Collects data once per tick
    bool CollectData(SensorDataType& Data) override
    {
        const auto CurrentTick = m_TickProvider();

        if (CurrentTick.Compare(m_LastTick) == TickPoint::CompareStatus::Equal) { return History(Data); }

        if (OnCollectData(Data))
        {
            m_LastTick = CurrentTick;
            return true;
        }
        return false;
    }

public:
    using TickProvider = TickPoint (*)();

    explicit ITickedSensor(TickProvider Provider) : m_TickProvider(Provider), m_LastTick(hal::Tick::Invalid())
    {
        assert(nullptr != Provider);
    }

    virtual ~ITickedSensor() = default;

protected:
    virtual bool OnCollectData(SensorDataType&) = 0;
    virtual bool History(SensorDataType&)       = 0;

protected:
    TickProvider m_TickProvider;
    TickPoint m_LastTick;
};
} // namespace ra
