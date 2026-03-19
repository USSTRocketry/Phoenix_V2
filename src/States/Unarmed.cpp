#include "States.h"
#include "Global.h"
#include "TypeConversion/TypeConversion.h"
#include "Util.h"

Unarmed::Unarmed()
{
    ra::Logger::LogInfo LogInfo{
        .Timestamp = ra::global::GetSysTick().Raw(),
        .Level     = ra::Logger::Severity::Info,
        .Category  = ra::type::Category::FlightControl,
    };

    ra::type::FlightControlMsg Message { .State = ToFlightState(GetState()) };
    ra::global::Logger.Log(LogInfo, Message);
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
