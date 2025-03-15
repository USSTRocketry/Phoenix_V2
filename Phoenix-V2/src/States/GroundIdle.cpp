#include "States.h"

#include <Arduino.h>

FlightState GroundIdle::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    
    if(true)
    {
        // transition to new state, will break SM if you create random obj
	    return MemPool.emplace<InFlight>().GetState();
    }
	return GetState();
}

FlightState inline GroundIdle::GetState() const
{
	return eGroundIdle;
}