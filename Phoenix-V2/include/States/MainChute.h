#ifndef FLIGHT_COMPUTER_MAINCHUTE_H
#define FLIGHT_COMPUTER_MAINCHUTE_H

#include "BaseState.h"

class MainChute : BaseState
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

    ~MainChute() override = default;
};
#endif //FLIGHT_COMPUTER_MAINCHUTE_H
