#include "Global.h"

namespace ra::global
{
bool ParachuteDeployed {false};
} // namespace ra::global

namespace ra::global::calibration
{
GroundNormalData GroundNormal {Eigen::Vector3f::Zero(), 0.f};
struct SensorData SensorData {};
} // namespace ra::global::calibration
