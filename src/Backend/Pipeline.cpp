#include "Backend/Pipeline.h"

#include "Backend/Device.h"
#include "Core/Vector.h"

namespace wind
{
    void GraphicsPipeline::Init()
    {
        // Create pipeline
        vk::Device                        vkDevice = m_device->GetDevice();
        vk::PipelineShaderStageCreateInfo shaderStages[2];

        // default dynamic state
        wind::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        m_dynamicState.dynamicStateCount             = static_cast<uint32_t>(dynamicStates.size());
        m_dynamicState.pDynamicStates                = dynamicStates.data();

        // get shader info and layout info from shader
        m_shader->PopShaderStageInfo(m_shaderStages);
        m_shader->PopPipelineLayoutInfo(m_layoutInfo);
    }

    void GraphicsPipeline::Destroy()
    {
        vk::Device vkDevice = m_device->GetDevice();
        vkDevice.destroyPipeline(m_pipeline);
    }
} // namespace wind