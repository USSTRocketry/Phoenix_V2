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

    constexpr auto Delta          = 2;
    float GroundRelativeMagnitude = calibration::GroundNormal.Direction.dot(Context.Sensors.AccelGyro.Accel);
    float LaunchMagRatio          = GroundRelativeMagnitude / calibration::GroundNormal.Magnitude;

    if (LaunchMagRatio > Delta)
    {
        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<InFlight>(Context.Sensors.BMP280.Altitude).GetState();
    }

    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }
