#include "States.h"
#include "Util.h"

FlightState Unarmed::Run(const StateContext& Context, FlightStateMemPool& MemPool)
{
    UnUsed(Context);

    // TODO: be able to receive arming signal
    if (true)
    {
        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<GroundIdle>().GetState();
    }
    return GetState();
}

FlightState Unarmed::GetState() const { return FlightState_Unarmed; }
