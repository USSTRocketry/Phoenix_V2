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
	 * Initialize and enter into Unarmed state
	 */
	explicit StateMachine();

	/**
	 * Performs state action
	 * @return The current state
	 */
	FlightState Run(const SensorData&);

	FlightState GetState() const;

	StateMachine(StateMachine&) = delete;

	StateMachine& operator =(StateMachine&) = delete;

private:
	FlightStateMemPool m_MemPool{};
};