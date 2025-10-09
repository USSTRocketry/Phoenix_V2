#pragma once
#include "BaseState.h"

class MainChute : public BaseState
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
    MainChute();
    ~MainChute() override = default;

private:
    uint32_t m_SteadyCounter {0};
};
