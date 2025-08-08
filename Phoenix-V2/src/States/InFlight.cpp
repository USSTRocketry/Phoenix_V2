#include "States.h"
#include "Global.h"

#include "SDHandler.h"

FlightState InFlight::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    // determine if we hit apogee
    Serial.println("Alt:");
    Serial.println(SensorData.BMP280.Altitude);

    Serial.println("m_Alt:");
    Serial.println(m_Apogee);

    constexpr uint32_t MinApogeeCount = 10;
    const auto& RefAlt                = SensorData.BMP280.Altitude;
    constexpr auto Epsilon            = 0.3f; // change back to 2 when done testing

    if (RefAlt > m_Apogee) {
        m_Apogee        = RefAlt;
        m_ApogeeCounter = 0;
    }
    else if((m_Apogee - RefAlt) > Epsilon){
            m_ApogeeCounter++;
            StoreStringLineToCSV("Alt low : Prev " + std::to_string(m_Apogee) + " New " +
                                std::to_string(SensorData.BMP280.Altitude));

            if (m_ApogeeCounter > MinApogeeCount)
            {
                StoreStringLineToCSV("Switching State");
                return MemPool.emplace<MainChute>().GetState();
            }
    }

    Serial.println("Apogee Counter: ");
    Serial.println(m_ApogeeCounter);

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }

InFlight::InFlight(float CurrentAltitude) : m_Apogee(CurrentAltitude)
{
    StoreStringLineToCSV("Current State : " + std::to_string(GetState()));
    StoreStringLineToCSV("BMP Alt : " + std::to_string(CurrentAltitude));
}
