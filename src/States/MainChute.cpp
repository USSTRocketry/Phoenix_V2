#include "States.h"
#include "Global.h"
#include "TypeConversion/TypeConversion.h"

MainChute::MainChute()
{
    ra::Logger::LogInfo LogInfo{
        .Timestamp = ra::global::GetSysTick().Raw(),
        .Level     = ra::Logger::Severity::Info,
        .Category  = ra::type::Category::FlightControl,
    };

    ra::type::FlightControlMsg Message { .State = ToFlightState(GetState()) };
    ra::global::Logger.Log(LogInfo, Message);
}

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
            ra::Logger::LogInfo LandedInfo{
                .Timestamp = ra::global::GetSysTick().Raw(),
                .Level     = ra::Logger::Severity::Info,
                .Category  = ra::type::Category::FlightControl,
            };
            ra::global::Logger.Log(LandedInfo, 0, "We've landed! turning off statemachine");
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
