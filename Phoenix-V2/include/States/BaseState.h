#ifndef FLIGHT_COMPUTER_BASESTATE_H
#define FLIGHT_COMPUTER_BASESTATE_H

#include "Sensor.h"
#include <variant>
#include <cstdint>

using FlightStateMemPool = std::variant<class Unarmed, class GroundIdle, class InFlight, class MainChute>;

enum FlightState : uint8_t
{
    eUnarmed,
    eGroundIdle,
    eInFlight,
    eMainChute
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
    [[nodiscard]] virtual FlightState inline GetState() const = 0;

    virtual ~BaseState() = default;
};

#endif // FLIGHT_COMPUTER_BASESTATE_H
