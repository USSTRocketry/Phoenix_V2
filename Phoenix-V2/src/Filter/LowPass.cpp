#include "Filter/LowPass.h"

namespace Filter
{
SensorData LowPass::Filter(const SensorData& NewData) { return m_History * m_Alpha + NewData * (1 - m_Alpha); }
} // namespace Filter
