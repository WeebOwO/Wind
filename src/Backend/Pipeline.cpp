#include "Backend/Pipeline.h"

#include "Backend/Device.h"
#include "Backend/VertexLayout.h"
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

        // input assembly state
        // todo: make topology configurable
        m_inputAssemblyState.setPrimitiveRestartEnable(false).setTopology(vk::PrimitiveTopology::eTriangleList);

        // input vertex state
        InitVertexInputState(VertexLayoutType::StaticMesh, m_vertexInputState);

        // rasterization state
        m_rasterizationState.setDepthClampEnable(false)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setLineWidth(1.0f)
            .setCullMode(vk::CullModeFlagBits::eNone)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthBiasEnable(false)
            .setDepthBiasConstantFactor(0.0f)
            .setDepthBiasClamp(0.0f)
            .setDepthBiasSlopeFactor(0.0f);

        // multi sample state
        m_multisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1);

        // depth stencil state
        m_depthStencilState.setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false);

        // color blend state
        m_colorBlendState.setAttachmentCount(1).setPAttachments(nullptr).setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

        // get shader info and layout info from shader
        m_shader->PopShaderStageInfo(m_shaderStages);
        m_shader->PopPipelineLayoutInfo(m_layoutInfo);

        // create pipeline
        m_createInfo.setStages(m_shaderStages).setLayout(m_pipelineLayout).setRenderPass(nullptr);

        m_createInfo.setRenderPass(nullptr)
            .setPInputAssemblyState(&m_inputAssemblyState)
            .setPVertexInputState(&m_vertexInputState)
            .setPMultisampleState(&m_multisampleState)
            .setPRasterizationState(&m_rasterizationState)
            .setPDepthStencilState(&m_depthStencilState)
            .setPColorBlendState(&m_colorBlendState)
            .setPDynamicState(&m_dynamicState)
            .setLayout(m_pipelineLayout);

        m_pipeline = vkDevice.createGraphicsPipeline(nullptr, m_createInfo, nullptr).value;
    }

    void GraphicsPipeline::Destroy()
    {
        vk::Device vkDevice = m_device->GetDevice();
        vkDevice.destroyPipelineLayout(m_pipelineLayout);
        vkDevice.destroyPipeline(m_pipeline);
    }
} // namespace wind