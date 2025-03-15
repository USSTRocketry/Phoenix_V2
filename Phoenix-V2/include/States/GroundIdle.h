#ifndef FLIGHT_COMPUTER_GROUNDIDLE_H
#define FLIGHT_COMPUTER_GROUNDIDLE_H

#include "BaseState.h"

class GroundIdle : BaseState
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

    ~GroundIdle() override = default;
};
#endif //FLIGHT_COMPUTER_GROUNDIDLE_H
