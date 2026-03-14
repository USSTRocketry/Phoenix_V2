#pragma once
#include "BaseState.h"

class GroundIdle : public BaseState
{
public:
    // main
    FlightState Run(const StateContext&, FlightStateMemPool&) override;

    /**
     * Finds the State of the current State
     * @return The state currently at
     */
    [[nodiscard]]
    FlightState GetState() const override;

public:
    GroundIdle()           = default;
    ~GroundIdle() override = default;
};
