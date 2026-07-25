#include "StateMachine.h"

StateMachine::StateMachine() { m_MemPool.emplace<Unarmed>(); }

FlightState StateMachine::Run(const StateContext& context)
{
    return std::visit([&](auto&& CurrentState) { return CurrentState.Run(context, m_MemPool); }, m_MemPool);
}

FlightState StateMachine::GetState() const
{
    return std::visit([&](const auto& CurrentState) { return CurrentState.GetState(); }, m_MemPool);
}
