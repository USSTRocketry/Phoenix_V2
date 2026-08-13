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
    constexpr uint8_t FlightTimeoutTicks = 25;    // 5 seconds timeout to verify altitude

    float GroundMag = calibration::GroundNormal.Magnitude;
    if (GroundMag < 5.0f) { GroundMag = 9.81f; } // Fallback to 1g baseline if uncalibrated

    float GroundRelativeMagnitude = calibration::GroundNormal.Direction.dot(Context.Sensors.AccelGyro.Accel);
    float LaunchMagRatio          = GroundRelativeMagnitude / GroundMag;

    if (!m_PadAltitudeInitialized)
    {
        m_PadAltitude = Context.Sensors.BMP280.Altitude;
        m_PadAltitudeInitialized = true;
    }

    if (!m_MotorBurnDetected)
    {
        if (LaunchMagRatio > Delta)
        {
            m_DebounceCount++;
            if (m_DebounceCount >= LaunchDebounceTicks)
            {
                m_MotorBurnDetected = true;
                m_FlightCheckTicks = 0;
            }
        }
        else
        {
            m_DebounceCount = 0;
            // Slowly track pad altitude to handle drift or driving around
            m_PadAltitude = (m_PadAltitude * 0.9f) + (Context.Sensors.BMP280.Altitude * 0.1f);
        }
    }
    else
    {
        m_FlightCheckTicks++;
        float AltitudeChange = Context.Sensors.BMP280.Altitude - m_PadAltitude;
        
        if (AltitudeChange > 10.0f)
        {
            return MemPool.emplace<InFlight>(m_PadAltitude).GetState();
        }
        else if (m_FlightCheckTicks >= FlightTimeoutTicks)
        {
            // False positive, reset
            m_MotorBurnDetected = false;
            m_DebounceCount = 0;
            m_FlightCheckTicks = 0;
        }
    }

    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }
