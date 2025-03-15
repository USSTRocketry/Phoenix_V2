#ifndef FLIGHT_COMPUTER_UNARMED_H
#define FLIGHT_COMPUTER_UNARMED_H

#include "BaseState.h"

class Unarmed : BaseState
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

	~Unarmed() override = default;
};

#endif //FLIGHT_COMPUTER_UNARMED_H
