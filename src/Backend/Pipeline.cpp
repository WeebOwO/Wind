#include "Backend/Pipeline.h"

#include "Backend/Device.h"
#include "Backend/Resource.h"

namespace wind
{
    PipelineBuilder::PipelineBuilder(Device* device) : m_device(device) {}

    PipelineBuilder& PipelineBuilder::ApplyDefaultState()
    {
        // input assembly state
        m_pipeInfo.inputAssemblyStateCI.setPrimitiveRestartEnable(false).setTopology(
            vk::PrimitiveTopology::eTriangleList);

        // vertex input state
        InitVertexInputState(VertexLayoutType::StaticMesh, m_pipeInfo.vertexInputStateCI);

        // rasterization state
        m_pipeInfo.rasterizationStateCI.setCullMode(vk::CullModeFlagBits::eNone)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setLineWidth(1.0f)
            .setDepthClampEnable(false)
            .setRasterizerDiscardEnable(false)
            .setDepthBiasEnable(false);

        // viewport and scissor
        m_pipeInfo.viewportStateCI.setViewportCount(1).setScissorCount(1).setPViewports(nullptr).setPScissors(nullptr);

        // dynamic state
        std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        m_pipeInfo.dynamicStateCI.setPDynamicStates(dynamicStates.data())
            .setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));

        // depth stencil state
        m_pipeInfo.depthStencilStateCI.setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false);

        // color blend state
        m_pipeInfo.colorBlendStateCI.setAttachmentCount(1)
            .setPAttachments(&m_pipeInfo.colorBlendAttachments[0])
            .setLogicOpEnable(false)
            .setLogicOp(vk::LogicOp::eCopy)
            .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

        // render pass
        m_pipeInfo.pipelineCI.setRenderPass(nullptr); // use dynamic rendering hh

        // multisample state
        m_pipeInfo.multisampleStateCI.setSampleShadingEnable(false)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setMinSampleShading(1.0f)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(false)
            .setAlphaToOneEnable(false);

        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetShaders(const std::span<Shader*>& shaders)
    {
        for (auto shader : shaders)
        {
            vk::PipelineShaderStageCreateInfo stageInfo;
            stageInfo.stage  = shader->GetStage();
            stageInfo.module = shader->GetModule();
            stageInfo.pName  = "main";

            m_pipeInfo.shaderStages.push_back(stageInfo);
        }

        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetVertexLayout(VertexLayoutType vertexLayout)
    {
        InitVertexInputState(vertexLayout, m_pipeInfo.vertexInputStateCI);
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetPassType(PassType passType)
    {
        if (passType == PassType::Lighting)
        {
            std::vector<vk::Format> colorFormats = {vk::Format::eR8G8B8A8Unorm};
            m_pipeInfo.renderInfoKHR.setDepthAttachmentFormat(vk::Format::eD32Sfloat)
                .setColorAttachmentCount(1)
                .setColorAttachmentFormats(colorFormats);
        }
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetBlendMode(BlendMode blendMode)
    {
        m_pipeInfo.colorBlendAttachments.clear();

        if (blendMode == BlendMode::Opaque)
        {
            vk::PipelineColorBlendAttachmentState colorBlendAttachment;
            colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            colorBlendAttachment.blendEnable = false;

            m_pipeInfo.colorBlendAttachments.push_back(colorBlendAttachment);
        }
        else if (blendMode == BlendMode::AlphaBlend)
        {
            vk::PipelineColorBlendAttachmentState colorBlendAttachment;
            colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            colorBlendAttachment.blendEnable         = true;
            colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachment.colorBlendOp        = vk::BlendOp::eAdd;
            colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            colorBlendAttachment.alphaBlendOp        = vk::BlendOp::eAdd;

            m_pipeInfo.colorBlendAttachments.push_back(colorBlendAttachment);
        }

        m_pipeInfo.colorBlendStateCI.setAttachmentCount(static_cast<uint32_t>(m_pipeInfo.colorBlendAttachments.size()))
            .setPAttachments(m_pipeInfo.colorBlendAttachments.data());
        return *this;
    }

    PipelineBuilder& PipelineBuilder::SetLayout(vk::PipelineLayout layout)
    {
        m_pipeInfo.pipelineCI.setLayout(layout);
        return *this;
    }

    std::shared_ptr<Pipeline> PipelineBuilder::Build()
    {
        m_pipeInfo.pipelineCI.setStageCount(static_cast<uint32_t>(m_pipeInfo.shaderStages.size()))
            .setPStages(m_pipeInfo.shaderStages.data())
            .setPVertexInputState(&m_pipeInfo.vertexInputStateCI)
            .setPInputAssemblyState(&m_pipeInfo.inputAssemblyStateCI)
            .setPRasterizationState(&m_pipeInfo.rasterizationStateCI)
            .setPColorBlendState(&m_pipeInfo.colorBlendStateCI)
            .setPDepthStencilState(&m_pipeInfo.depthStencilStateCI)
            .setPMultisampleState(&m_pipeInfo.multisampleStateCI)
            .setPDynamicState(&m_pipeInfo.dynamicStateCI)
            .setPViewportState(&m_pipeInfo.viewportStateCI)
            .setPNext(&m_pipeInfo.renderInfoKHR);

        return m_device->CreateResource<Pipeline>(m_pipeInfo.pipelineCI);
    }

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