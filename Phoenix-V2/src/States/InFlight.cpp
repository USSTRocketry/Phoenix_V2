#include "States.h"
#include "Global.h"

FlightState InFlight::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    constexpr uint32_t MinApogeeCount = 20;
    constexpr auto Epsilon            = 2;
    const auto& RefAlt                = SensorData.BMP280.Altitude;

    if (RefAlt > m_Apogee)
    {
        m_Apogee        = RefAlt;
        m_ApogeeCounter = 0;
    }
    else if ((m_Apogee - RefAlt) > Epsilon)
    {
        m_ApogeeCounter++;

        if (m_ApogeeCounter > MinApogeeCount) { return MemPool.emplace<MainChute>().GetState(); }
    }

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }
