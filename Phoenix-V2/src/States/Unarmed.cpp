#include "States.h"
#include "Util.h"
#include "SDHandler.h"

FlightState Unarmed::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    UnUsed(SensorData);

    // TODO: be able to receive arming signal
    if (true)
    {
        StoreStringLineToCSV("Switching State");
        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<GroundIdle>().GetState();
    }
    return GetState();
}

FlightState Unarmed::GetState() const { return FlightState_Unarmed; }

Unarmed::Unarmed() { StoreStringLineToCSV("Current State : " + std::to_string(GetState())); }
