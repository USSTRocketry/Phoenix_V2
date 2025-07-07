#include "States.h"
#include "Global.h"

FlightState InFlight::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    // determine if we hit apogee
    if (calibration::SensorData.BMP280.Altitude > SD.BMP280.Altitude)
    {
        return MemPool.emplace<MainChute>().GetState();
    }
    return GetState();
}

FlightState InFlight::GetState() const { return FlightState_InFlight; }
