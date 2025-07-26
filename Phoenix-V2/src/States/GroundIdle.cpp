#include "States.h"
#include "Global.h"

FlightState GroundIdle::Run(const SensorData& SD, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    // we might need to normalize the vectors before we dot product
    // as a hack make the dot product abs value. BAD !!!!
    if (calibration::GroundNormal.dot(SD.AccelGyroData.Accel) > 2)
    {
        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<InFlight>(SD.BMP280.Altitude).GetState();
    }
    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }
