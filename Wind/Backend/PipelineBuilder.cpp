#include "PipelineBuilder.h"

#include "Device.h"
#include "RasterShader.h"

#include "Engine/RuntimeContext.h"
#include "Resource/VertexFactory.h"

namespace wind
{
    PipelineBuilder& PipelineBuilder::SetInputAssemblyState(vk::PrimitiveTopology topology, bool primitiveRestartEnable)
    {
        m_inputAssemblyStateInfo = vk::PipelineInputAssemblyStateCreateInfo {
            .topology = topology, .primitiveRestartEnable = primitiveRestartEnable};
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetVertexType(EVertexType vertexType)
    {
        switch (vertexType)
        {
            case EVertexType::NoVertex: {
                break;
            };
            case EVertexType::StaticMesh: {
                m_inputBindingDescriptions   = StaticMeshVertexFactory::GetInputBindingDescription();
                m_inputAttributeDescriptions = StaticMeshVertexFactory::GetVertexInputAttributeDescriptions();
                m_inputStateCreateInfo.setVertexBindingDescriptions(m_inputBindingDescriptions)
                    .setVertexAttributeDescriptions(m_inputAttributeDescriptions);
                break;
            };
        }
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetRasterizationState(vk::PolygonMode   polygonMode,
                                                            vk::CullModeFlags cullMode,
                                                            vk::FrontFace     frontFace)
    {
        m_rasterizationStateInfo = vk::PipelineRasterizationStateCreateInfo {.depthClampEnable        = false,
                                                                             .rasterizerDiscardEnable = false,
                                                                             .polygonMode             = polygonMode,
                                                                             .cullMode                = cullMode,
                                                                             .frontFace               = frontFace,
                                                                             .lineWidth               = 1.0f};

        return *this;
    }

    void PipelineBuilder::SetShaderState(const RasterShader& shader)
    {
        m_shaderStages[0]
            .setModule(shader.GetVertexModule())
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setPName("main");

        m_shaderStages[1]
            .setModule(shader.GetFragModule())
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setPName("main");

        m_layout = shader.GetPipelineLayout();
    }

    PipelineBuilder& PipelineBuilder::SetRenderState(const RenderState& renderState)
    {
        bool blendEnable      = renderState.blendState.blendEnable;
        bool depthWriteEnable = renderState.depthState.detphWrite;
        bool depthTest        = renderState.depthState.depthTest;

        uint8_t mrtCount  = renderState.blendState.mrtCount;
        m_attachmentState = std::vector<vk::PipelineColorBlendAttachmentState>(
            mrtCount,
            vk::PipelineColorBlendAttachmentState {
                .blendEnable    = blendEnable,
                .colorWriteMask = BLEND_ALL_FLAG}); // todo : change this state with different blend mode

        m_blendStateInfo.setAttachments(m_attachmentState).setAttachmentCount(m_attachmentState.size());

        m_depthStencilState.setMinDepthBounds(0.0f)
            .setMaxDepthBounds(1.0f)
            .setDepthWriteEnable(depthWriteEnable)
            .setDepthTestEnable(true)
            .setStencilTestEnable(false)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual);
            
        if (renderState.rasterShader != nullptr)
            SetShaderState(*renderState.rasterShader);
        return *this;
    }

    vk::Pipeline PipelineBuilder::Build()
    {
        // dynamic state
        std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

        vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
        pipelineDynamicStateCreateInfo.setDynamicStates(dynamicStates);

        // multi sample state
        vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        multisampleStateCreateInfo.setSampleShadingEnable(false).setRasterizationSamples(vk::SampleCountFlagBits::e1);

        // viewportinfo
        vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo;
        viewPortStateCreateInfo.setViewportCount(1).setScissorCount(1);

        // using dynamic rendering to get shit renderpass
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo {.stageCount          = 2,
                                                           .pStages             = m_shaderStages.data(),
                                                           .pVertexInputState   = &m_inputStateCreateInfo,
                                                           .pInputAssemblyState = &m_inputAssemblyStateInfo,
                                                           .pViewportState      = &viewPortStateCreateInfo,
                                                           .pRasterizationState = &m_rasterizationStateInfo,
                                                           .pMultisampleState   = &multisampleStateCreateInfo,
                                                           .pDepthStencilState  = &m_depthStencilState,
                                                           .pColorBlendState    = &m_blendStateInfo,
                                                           .pDynamicState       = &pipelineDynamicStateCreateInfo,
                                                           .layout              = m_layout,
                                                           .renderPass          = nullptr};

        auto vkDevice = g_runtimeContext.device->vkDevice();
        return vkDevice.createGraphicsPipelines(nullptr, pipelineCreateInfo).value.front();
    }
} // namespace wind