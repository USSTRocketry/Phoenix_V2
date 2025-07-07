#pragma once

#include "Sensor.h"
#include <variant>
#include <cstdint>

using FlightStateMemPool = std::variant<class Unarmed, class GroundIdle, class InFlight, class MainChute>;

enum FlightState : uint8_t
{
    FlightState_Unknown,
    FlightState_Unarmed,
    FlightState_GroundIdle,
    FlightState_InFlight,
    FlightState_MainChute
};

struct BaseState
{
    /**
     * Performs task
     * @return the current state
     */
    virtual FlightState Run(const SensorData&, FlightStateMemPool&) = 0;

    /**
     * Finds the State of the current State
     * @return The state currently at
     */
    [[nodiscard]] virtual FlightState GetState() const = 0;

    virtual ~BaseState() = default;
};
