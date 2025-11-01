#include "States.h"
#include "Global.h"

FlightState InFlight::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    constexpr uint32_t MinApogeeCount = 20;
    constexpr auto Epsilon            = 0.3f;
    const auto& CurrentAlt            = SensorData.BMP280.Altitude;

    Serial.println("Current Altitude");
    Serial.println(CurrentAlt);

    Serial.println("Apogee Counter");
    Serial.println(m_ApogeeCounter);

    Serial.println("Apogee");
    Serial.println(m_Apogee);
    
    if (CurrentAlt > m_Apogee)
    {
        m_Apogee        = CurrentAlt;
        m_ApogeeCounter = 0;
    }
    else if ((m_Apogee - CurrentAlt) > Epsilon)
    {
        Serial.println("Apogee difference > Epsilon");

        m_ApogeeCounter++;

        if (m_ApogeeCounter > MinApogeeCount) { return MemPool.emplace<MainChute>().GetState(); }
    }

    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }
