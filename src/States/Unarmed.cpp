#include "States.h"
#include "Global.h"
#include "TypeConversion/TypeConversion.h"
#include "Util.h"

Unarmed::Unarmed()
{
}

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
