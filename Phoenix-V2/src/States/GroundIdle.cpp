#include "States.h"
#include "Global.h"

#include "SDHandler.h"

FlightState GroundIdle::Run(const SensorData& SensorData, FlightStateMemPool& MemPool)
{
    using namespace ra::global;

    // we might need to normalize the vectors before we dot product
    // as a hack make the dot product abs value. BAD !!!!
    if (calibration::GroundNormal.dot(SensorData.AccelGyroData.Accel) > 2)
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
