#include "States.h"
#include "Global.h"

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
