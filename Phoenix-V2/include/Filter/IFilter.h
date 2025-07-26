#pragma once

#include "SensorInterface.h"

namespace Filter
{
template <typename FilteredType>
class IFilter
{
public:
    virtual FilteredType Filter(const SensorData& NewData) = 0;

public:
    virtual ~IFilter() = default;
};
} // namespace Filter
