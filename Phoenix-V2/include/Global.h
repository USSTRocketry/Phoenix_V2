#pragma once

#include <cstddef>
#include "ArduinoEigen.h"
#include "SensorInterface.h"

namespace ra::global
{
extern bool ParachuteDeployed;
constexpr size_t SensorCount = 10;
} // namespace ra::global

// "ground truth" calibration values
namespace ra::global::calibration
{
struct GroundNormalData
{
    Eigen::Vector3f Direction;
    float Magnitude;
};
extern GroundNormalData GroundNormal;

extern SensorData SensorData;
} // namespace ra::global::calibration
