#pragma once
#include <Arduino.h>
#include "States.h"

/**
 * Manages States
 */
class StateMachine
{
public:
    /**
     * Performs state action
     * @return The current state
     */
    FlightState Run(const SensorData&);

    FlightState GetState() const;

    // State better be a valid state
    template <typename State, typename... Arg>
    FlightState EnterState(Arg&&... Args)
    {
        return m_MemPool.emplace<State>(std::forward<Arg>(Args)...).GetState();
    }

public:
    /**
     * Initialize and enter into Unarmed state
     */
    explicit StateMachine();

    StateMachine(StateMachine&)            = delete;
    StateMachine& operator=(StateMachine&) = delete;

private:
    FlightStateMemPool m_MemPool {};
};
