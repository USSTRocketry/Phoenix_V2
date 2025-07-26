#include "States.h"
#include "Global.h"

#include "SDHandler.h"

FlightState InFlight::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    // determine if we hit apogee
    if (m_Altitude > SensorData.BMP280.Altitude)
    {
        m_ApogeeCounter++;
        StoreStringLineToCSV("Alt low : Prev " + std::to_string(m_Altitude) + " New " +
                             std::to_string(SensorData.BMP280.Altitude));

        if (m_ApogeeCounter < m_MinApogeeCount)
        {
            StoreStringLineToCSV("Switching State");
            return MemPool.emplace<MainChute>().GetState();
        }
    }
    else { m_Altitude = SensorData.BMP280.Altitude; }

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }

InFlight::InFlight(float CurrentAltitude) : m_Altitude(CurrentAltitude)
{
    StoreStringLineToCSV("Current State : " + std::to_string(GetState()));
    StoreStringLineToCSV("BMP Alt : " + std::to_string(CurrentAltitude));
}
