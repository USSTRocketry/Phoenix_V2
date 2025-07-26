#include "States.h"
#include "Global.h"

FlightState InFlight::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    // determine if we hit apogee
    if (m_Altitude > SD.BMP280.Altitude)
    {
        m_ApogeeCounter++;
        if (m_ApogeeCounter < m_MinApogeeCount) { return MemPool.emplace<MainChute>().GetState(); }
    }

    m_Altitude = SD.BMP280.Altitude;
    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }
