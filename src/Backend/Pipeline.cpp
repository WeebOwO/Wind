#include "Backend/Pipeline.h"

#include "Backend/Device.h"
#include "Backend/Resource.h"

namespace wind
{
    Pipeline::Pipeline(Device* device, const vk::GraphicsPipelineCreateInfo& createInfo) :
        Resource(device, Tag::Pipeline), m_type(PipelineType::Graphics)
    {
        m_pipeline = device->GetDevice().createGraphicsPipeline(nullptr, createInfo).value;
    }

    Pipeline::Pipeline(Device* device, const vk::ComputePipelineCreateInfo& createInfo) :
        Resource(device, Tag::Pipeline), m_type(PipelineType::Compute)
    {
        m_pipeline = device->GetDevice().createComputePipeline(nullptr, createInfo).value;
    }

    Pipeline::~Pipeline()
    {
        vk::Device device = m_device->GetDevice();
        device.destroyPipeline(m_pipeline);
    }
} // namespace wind