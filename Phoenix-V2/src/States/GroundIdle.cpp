#include "States.h"
#include "Global.h"

#include "SDHandler.h"

FlightState GroundIdle::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    constexpr auto Delta          = 2;
    float GroundRelativeMagnitude = calibration::GroundNormal.Direction.dot(SensorData.AccelGyroData.Accel);

    if (GroundRelativeMagnitude > Delta)
    {
        StoreStringLineToCSV("Switching State");
        StoreStringLineToCSV("Accel X : " + std::to_string(SensorData.AccelGyroData.Accel.x()) + " Y " +
                             std::to_string(SensorData.AccelGyroData.Accel.y()) + " Z " +
                             std::to_string(SensorData.AccelGyroData.Accel.z()));

        // transition to new state, will break SM if you create random obj
        return MemPool.emplace<InFlight>(SensorData.BMP280.Altitude).GetState();
    }

    return GetState();
}

FlightState GroundIdle::GetState() const { return FlightState_GroundIdle; }

GroundIdle::GroundIdle() { StoreStringLineToCSV("Current State : " + std::to_string(GetState())); }
