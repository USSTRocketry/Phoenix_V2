#include "States.h"
#include "Global.h"

#include "SDHandler.h"

FlightState InFlight::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    // determine if we hit apogee
    Serial.println("Alt:");
    Serial.println(SensorData.BMP280.Altitude);

    Serial.println("m_Alt:");
    Serial.println(m_Altitude);
    
    // m_Altitude increases/decreases with BMP280 altitude. If case is never true. Altitude is sensitive so can't compare directly,
    // need a factor, but the factor doesn't factor due to the small steps.
    if (m_Altitude > SensorData.BMP280.Altitude)
    {
        // if (m_Altitude > SensorData.BMP280.Altitude)
        m_ApogeeCounter++;
        StoreStringLineToCSV("Alt low : Prev " + std::to_string(m_Altitude) + " New " +
                             std::to_string(SensorData.BMP280.Altitude));

        if (m_ApogeeCounter > m_MinApogeeCount)
        {
            StoreStringLineToCSV("Switching State");
            return MemPool.emplace<MainChute>().GetState();
        }
    }
    else { 
        m_Altitude = SensorData.BMP280.Altitude;
        m_ApogeeCounter = 0; 
    }

    Serial.println("Apogee Counter: ");
    Serial.println(m_ApogeeCounter);

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }

InFlight::InFlight(float CurrentAltitude) : m_Altitude(CurrentAltitude)
{
    StoreStringLineToCSV("Current State : " + std::to_string(GetState()));
    StoreStringLineToCSV("BMP Alt : " + std::to_string(CurrentAltitude));
}
