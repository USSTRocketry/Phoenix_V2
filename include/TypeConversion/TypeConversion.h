#pragma once

#include "Type.h"
#include "States/BaseState.h"

inline constexpr ra::type::FlightState ToFlightState(FlightState State)
{
    switch (State)
    {
        case FlightState_Unarmed:
            return ra::type::FlightState::Unarmed;
        case FlightState_GroundIdle:
            return ra::type::FlightState::GroundIdle;
        case FlightState_InFlight:
            return ra::type::FlightState::InFlight;
        case FlightState_MainChute:
            return ra::type::FlightState::MainChute;
        default:
            return ra::type::FlightState::Unknown;
    }
}


