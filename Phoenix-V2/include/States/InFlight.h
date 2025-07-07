#pragma once
#include "BaseState.h"

class InFlight : public BaseState
{
public:
    // main
    FlightState Run(const SensorData&, FlightStateMemPool&) override;

    /**
     * Finds the State of the current State
     * @return The state currently at
     */
    [[nodiscard]]
    FlightState GetState() const override;

    ~InFlight() override = default;
};
