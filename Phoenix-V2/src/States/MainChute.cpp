#include "States.h"

FlightState MainChute::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    if (true)
    {
        // transition to new state, will break SM if you create random obj
        // return MemPool.emplace<Unarmed>().GetState();
    }
    return GetState();
}

FlightState inline MainChute::GetState() const { return eMainChute; }
