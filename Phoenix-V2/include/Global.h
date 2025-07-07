#pragma once

#include <cstddef>
#include <Sensor.h>
#include "ArduinoEigen.h"

namespace ra::global
{
constexpr size_t SensorCount = 10;
} // namespace ra::global

namespace ra::global::calibration
{
static Eigen::Vector3f GroundNormal {};
static SensorData SensorData {};
} // namespace ra::global::calibration
