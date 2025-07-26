#pragma once

#include "BaseState.h"

class Unarmed : public BaseState
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

public:
    Unarmed();
    ~Unarmed() override = default;
};
