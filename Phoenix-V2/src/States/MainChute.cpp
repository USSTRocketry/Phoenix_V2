#include "States.h"

FlightState MainChute::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    digitalWrite(18, HIGH);
    delay(2000);
    digitalWrite(18, LOW);
    if (true)
    {
        // transition to new state, will break SM if you create random obj
        // return MemPool.emplace<Unarmed>().GetState();
    }
    return GetState();
}

FlightState inline MainChute::GetState() const { return eMainChute; }
