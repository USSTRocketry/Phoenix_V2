#include "States.h"

#include <Arduino.h>

FlightState Unarmed::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    //swichs states if recieved arming signal
    //TODO: be able to recieve arming signal
    if(true)
    {
        // transition to new state, will break SM if you create random obj
	   return MemPool.emplace<GroundIdle>().GetState();
    }
	return GetState();
}

FlightState inline Unarmed::GetState() const
{
	return eUnarmed;
}