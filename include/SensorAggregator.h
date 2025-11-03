#pragma once

#include <cstddef>
#include <vector>
#include <span>
#include <utility>
#include <algorithm>

#include "SensorInterface.h"

namespace ra
{
/**
 * @brief Aggregates multiple sensors of a common return type and provides
 *        mechanisms to collect data, apply functions, and manage storage.
 *
 * @tparam SensorReturnType The data type returned by each sensor's CollectData() method.
 *
 * This class acts as a manager for a collection of sensors. It allows:
 *   - Adding sensors dynamically (without taking ownership),
 *   - Applying arbitrary functions to all sensors,
 *   - Collecting data from all sensors with failure handling,
 *   - Storing the most recently successfully collected data,
 *   - Reserving and stabilizing storage for embedded or resource-constrained systems.
 *
 * Example usage:
 * @code
 * SensorAggregator<int> aggregator(10);  // reserve space for 10 sensors
 * aggregator.AddSensor(&sensor1);
 * aggregator.AddSensor({&sensor2, &sensor3});
 * auto [success, data] = aggregator.Collect();
 * aggregator.Apply([](auto* s){ s->Reset(); });
 * int lastData = aggregator.History();
 * @endcode
 */
template <typename SensorReturnType>
class SensorAggregator
{
    using ISensorType = ISensor<SensorReturnType>;

public:
    /**
     * @brief Add a single sensor to the aggregator.
     * @param s Pointer to a sensor (does not claim ownership)
     */
    void AddSensor(ISensorType* s) { m_SensorList.push_back(s); }
    /**
     * @brief Add multiple sensors using an initializer list.
     * @param S List of sensor pointers
     */
    void AddSensor(const std::span<ISensorType*>& S)
    {
        std::ranges::for_each(S, [this](const auto& s) { AddSensor(s); });
    }

    /**
     * @brief Apply an arbitrary callable (lambda, function, functor) to all sensors.
     * @param F Callable taking ISensorType* as argument
     */
    void Apply(auto&& F) { std::ranges::for_each(m_SensorList, F); }

    /* @brief Collect data from all sensors.
     *
     * @return std::pair<bool, SensorReturnType>
     *         - bool:              true if all sensors collected data successfully, false otherwise
     *         - SensorReturnType:  the newly collected data if successful, otherwise the last successful data
     */
    std::pair<bool, SensorReturnType> Collect()
    {
        SensorReturnType NewData;
        bool Result = std::ranges::all_of(m_SensorList, [&](auto* s) { return s->CollectData(NewData); });

        if (Result) { m_SensorData = NewData; }
        return {Result, m_SensorData};
    }
    /**
     * @brief Returns the last successfully collected data.
     * @return SensorReturnType The most recent successful collection
     */
    SensorReturnType History() { return m_SensorData; }

    /**
     * @brief Finalizes internal storage to potentially reduce memory usage.
     */
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
} // namespace ra
