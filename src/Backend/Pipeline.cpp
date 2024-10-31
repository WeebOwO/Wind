#include "Backend/Pipeline.h"

#include "Backend/Device.h"

namespace wind
{
    Pipeline::~Pipeline()
    {
        vk::Device device = m_device->GetDevice();
        device.destroyPipeline(m_pipeline);
    }
} // namespace wind