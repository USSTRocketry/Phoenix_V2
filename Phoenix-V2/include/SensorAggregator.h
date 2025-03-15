#pragma once

#include "Sensor.h"
#include <cstddef>
#include <array>
#include <vector>
#include <utility>

// array of object with Sensor interface, maybe use std::vector instead
class SensorAggregator
{
public:
    SensorAggregator();
    SensorAggregator(size_t Reserve) { m_SensorList.reserve(Reserve); }

    void AddSensor(Sensor* s) { m_SensorList.push_back(s); }

    void AddSensor(const std::vector<Sensor*>& S)
    {
        for (const auto& s : S)
        {
            m_SensorList.push_back(&s);
        }
        return true;
    }

    // on failure, returns the previous successfully collected data
    std::pair<bool, SensorData> Collect()
    {
        SensorData NewData;
        for (const auto& s : m_SensorList)
        {
            if (!s->CollectData(NewData))
            {
                return {false, m_SensorData};
            }
        }
        m_SensorData = NewData;
        return {true, m_SensorData};
    }

    // finalizes the structure of the underlying storage type, potentially save storage
    void Stabilitze() { m_SensorList.shrink_to_fit(); }

    SensorAggregator operator=(SensorAggregator&) = delete;
    SensorAggregator(SensorAggregator&)           = delete;

private:
    std::vector<Sensor*> m_SensorList;
    SensorData m_SensorData {};
};
