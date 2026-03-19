#include "States.h"
#include "Global.h"
#include "TypeConversion/TypeConversion.h"

InFlight::InFlight(float CurrentAltitude)
    : m_Apogee(CurrentAltitude)
{
    ra::Logger::LogInfo LogInfo{
        .Timestamp = ra::global::GetSysTick().Raw(),
        .Level     = ra::Logger::Severity::Info,
        .Category  = ra::type::Category::FlightControl,
    };

    ra::type::FlightControlMsg Message { .State = ToFlightState(GetState()) };
    ra::global::Logger.Log(LogInfo, Message);
}

FlightState InFlight::Run(const StateContext& Context, FlightStateMemPool& MemPool)
{
    constexpr uint32_t MinApogeeCount = 20;
    constexpr auto Epsilon            = 2;
    const auto& CurrentAlt            = Context.Sensors.BMP280.Altitude;

    if (CurrentAlt > m_Apogee)
    {
        m_Apogee        = CurrentAlt;
        m_ApogeeCounter = 0;
    }
    else if ((m_Apogee - CurrentAlt) > Epsilon)
    {
        m_ApogeeCounter++;

        if (m_ApogeeCounter > MinApogeeCount) { return MemPool.emplace<MainChute>().GetState(); }
    }

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }
