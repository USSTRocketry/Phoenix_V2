#include "States.h"
#include "Global.h"

FlightState MainChute::Run(const SensorData& SD, FlightStateMemPool&)
{
    constexpr auto Epsilon = 10;
    auto Norm              = SD.AccelGyroData.Accel.norm();
    auto Diff              = abs(ra::global::calibration::SensorData.AccelGyroData.Accel.norm() - Norm);

    if (Diff < Epsilon)
    {
        m_SteadyCounter++;
        if (m_SteadyCounter > 10)
        {
            // Sleep
        }
    }
    else { m_SteadyCounter = 0; }
    return GetState();
}

FlightState MainChute::GetState() const { return FlightState_MainChute; }

MainChute::MainChute() { ra::global::ParachuteDeployed = true; }
