#include "States.h"
#include "Global.h"
#include "SDHandler.h"

FlightState MainChute::Run(const SensorData& SensorData, FlightStateMemPool&)
{
    constexpr auto Epsilon        = 10;
    constexpr auto MaxSteadyCount = 50;
    // consider using BMP altitude
    auto Norm                     = SensorData.AccelGyroData.Accel.norm();
    auto Diff                     = abs(ra::global::calibration::SensorData.AccelGyroData.Accel.norm() - Norm);

    if (Diff < Epsilon)
    {
        m_SteadyCounter++;
        if (m_SteadyCounter > MaxSteadyCount)
        {
            // Sleep
            StoreStringLineToCSV("FC steady, enter sleep");
        }
    }
    else { m_SteadyCounter = 0; }
    return GetState();
}

FlightState MainChute::GetState() const { return FlightState_MainChute; }

MainChute::MainChute() { ra::global::ParachuteDeployed = true; }
