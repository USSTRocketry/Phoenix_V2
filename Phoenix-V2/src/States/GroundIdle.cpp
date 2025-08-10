#include "States.h"
#include "Global.h"

FlightState GroundIdle::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    constexpr auto Delta          = 2;
    float GroundRelativeMagnitude = calibration::GroundNormal.Direction.dot(SensorData.AccelGyroData.Accel);

    if (GroundRelativeMagnitude > Delta)
    {
        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<InFlight>(SensorData.BMP280.Altitude).GetState();
    }

    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }
