#pragma once

#include "SensorInterface.h"
#include <cstddef>
#include <vector>
#include <utility>

// array of object with Sensor interface, maybe use std::vector instead
template <typename SensorReturnType>
class SensorAggregator
{
    using ISensorType = ISensor<SensorReturnType>;

public:
    // Track sensors, does not claim ownership
    void AddSensor(ISensorType* s) { m_SensorList.push_back(s); }

    void AddSensor(const std::initializer_list<ISensorType*>& S)
    {
        for (const auto& s : S)
        {
            AddSensor(s);
        }
    }

    // apply arbitrary method over all sensors
    void Apply(auto&& F) { std::for_each(m_SensorList.begin(), m_SensorList.end(), F); }

    // on failure, returns the previous successfully collected data
    std::pair<bool, SensorReturnType> Collect()
    {
        SensorReturnType NewData;
        for (const auto& s : m_SensorList)
        {
            if (!s->CollectData(NewData)) { return {false, m_SensorData}; }
        }
        m_SensorData = NewData;
        return {true, m_SensorData};
    }

    SensorReturnType History() { return m_SensorData; }

    // finalizes the structure of the underlying storage type, potentially save storage
    void Stabilitze() { m_SensorList.shrink_to_fit(); }

public:
    template <typename... Sensors>
    explicit SensorAggregator(Sensors*... sensors) : m_SensorList {sensors...}
    {
        Stabilitze();
    }
    SensorAggregator(size_t Reserve) { m_SensorList.reserve(Reserve); }

    SensorAggregator operator=(SensorAggregator&) = delete;
    SensorAggregator(SensorAggregator&)           = delete;

private:
    std::vector<ISensorType*> m_SensorList;
    SensorReturnType m_SensorData {};
};
