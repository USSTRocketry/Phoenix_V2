#include "Filter/LowPass.h"

namespace Filter
{
    SensorData LowPass::Filter(const SensorData& NewData) { 
        m_History = m_History * m_Alpha + NewData * (1 - m_Alpha); 
        return m_History;
    }
} // namespace Filter
