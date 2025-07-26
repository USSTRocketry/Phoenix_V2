#include "Global.h"

namespace ra::global
{
bool ParachuteDeployed {false};
} // namespace ra::global

namespace ra::global::calibration
{
Eigen::Vector3f GroundNormal {};
struct SensorData SensorData {};
} // namespace ra::global::calibration
