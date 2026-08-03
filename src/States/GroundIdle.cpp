#include "States.h"
#include "Global.h"
#include "TypeConversion/TypeConversion.h"

GroundIdle::GroundIdle()
{
    ra::Logger::LogInfo LogInfo{
        .Timestamp = ra::global::GetSysTick().Raw(),
        .Level     = ra::Logger::Severity::Info,
        .Category  = ra::type::Category::FlightControl,
    };

    ra::type::FlightControlMsg Message { .State = ToFlightState(GetState()) };
    ra::global::Logger.Log(LogInfo, Message);
}

FlightState GroundIdle::Run(const StateContext& Context, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    constexpr float Delta               = 2.0f; // 2x baseline gravity threshold
    constexpr uint8_t LaunchDebounceTicks = 3;    // Require 3 consecutive ticks above threshold

    float GroundMag = calibration::GroundNormal.Magnitude;
    if (GroundMag < 5.0f) { GroundMag = 9.81f; } // Fallback to 1g baseline if uncalibrated

    float GroundRelativeMagnitude = calibration::GroundNormal.Direction.dot(Context.Sensors.AccelGyro.Accel);
    float LaunchMagRatio          = GroundRelativeMagnitude / GroundMag;

    if (LaunchMagRatio > Delta)
    {
        m_DebounceCount++;
        if (m_DebounceCount >= LaunchDebounceTicks)
        {
            return MemPool.emplace<InFlight>(Context.Sensors.BMP280.Altitude).GetState();
        }
    }
    else
    {
        m_DebounceCount = 0;
    }

    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }
