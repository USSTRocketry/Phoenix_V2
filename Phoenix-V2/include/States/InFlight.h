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

public:
    explicit InFlight(float CurrentAltitude);
    ~InFlight() override = default;

private:
    static constexpr uint32_t m_MinApogeeCount = 10;
    uint32_t m_ApogeeCounter {};
    float m_Altitude;
};
