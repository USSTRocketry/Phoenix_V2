#include "States.h"
#include "Global.h"

FlightState MainChute::Run(const StateContext& Context, FlightStateMemPool&)
{
    constexpr auto Epsilon        = 10;
    constexpr auto MaxSteadyCount = 50;
    // consider using BMP altitude
    auto Norm                     = Context.Sensors.AccelGyro.Accel.norm();
    auto Diff                     = abs(ra::global::calibration::SensorData.AccelGyro.Accel.norm() - Norm);

    if (Diff < Epsilon)
    {
        m_SteadyCounter++;
        if (m_SteadyCounter > MaxSteadyCount)
        {
            // self cancel
            ra::global::MainQueue.Cancel(Context.FlightControlHandle);
            ra::global::Logger.Log({}, "We've landed! turning off statemachine");
            return GetState();
        }
    }
    else
    {
        m_SteadyCounter = 0;
    }
    return GetState();
}

FlightState MainChute::GetState() const { return FlightState_MainChute; }

MainChute::MainChute() = default;
