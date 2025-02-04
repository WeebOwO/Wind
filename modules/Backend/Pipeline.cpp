#include "Pipeline.h"

#include "Device.h"

namespace wind
{
    GraphicPipelineDesc::GraphicPipelineDesc() { Clear(); }

    void GraphicPipelineDesc::Clear()
    {
        m_ShaderStagesCI.clear();
        m_InputAssemblyCI        = {};
        m_RasterizerCI           = {};
        m_ColorBlendAttachmentCI = {};
        m_MultisamplingCI        = {};
        m_DepthStencilCI         = {};
        m_RenderingCI            = {};
        m_ColorFormat            = vk::Format::eUndefined;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader)
    {
        m_ShaderStagesCI.clear();
        m_ShaderStagesCI.push_back(
            {.stage = vk::ShaderStageFlagBits::eVertex, .module = vertexShader, .pName = "main"});
        m_ShaderStagesCI.push_back(
            {.stage = vk::ShaderStageFlagBits::eFragment, .module = fragmentShader, .pName = "main"});
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetInputTopology(vk::PrimitiveTopology topology)
    {
        m_InputAssemblyCI = {.topology = topology};
        m_InputAssemblyCI.setPrimitiveRestartEnable(false);
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetPolygonMode(vk::PolygonMode mode)
    {
        m_RasterizerCI = {.polygonMode = mode, .lineWidth = 1.0f};
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetCullMode(vk::CullModeFlags mode, vk::FrontFace frontFace)
    {
        m_RasterizerCI.setCullMode(mode);
        m_RasterizerCI.setFrontFace(frontFace);
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetMultisamplingNone()
    {
        m_MultisamplingCI = {
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable  = VK_FALSE,
            .minSampleShading     = 1.0f,
            .pSampleMask          = nullptr,
            // no alpha to coverage
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE,
        };
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetBlendMode(BlendMode mode)
    {
        if (mode == BlendMode::None)
        {
            m_ColorBlendAttachmentCI.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            m_ColorBlendAttachmentCI.blendEnable = VK_FALSE;
        }
        else if (mode == BlendMode::AlphaBlend)
        {
            m_ColorBlendAttachmentCI.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            m_ColorBlendAttachmentCI.blendEnable         = VK_TRUE;
            m_ColorBlendAttachmentCI.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            m_ColorBlendAttachmentCI.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            m_ColorBlendAttachmentCI.colorBlendOp        = vk::BlendOp::eAdd;
            m_ColorBlendAttachmentCI.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            m_ColorBlendAttachmentCI.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            m_ColorBlendAttachmentCI.alphaBlendOp        = vk::BlendOp::eAdd;
        }
        else if (mode == BlendMode::Additive)
        {
            m_ColorBlendAttachmentCI.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            m_ColorBlendAttachmentCI.blendEnable         = VK_TRUE;
            m_ColorBlendAttachmentCI.srcColorBlendFactor = vk::BlendFactor::eOne;
            m_ColorBlendAttachmentCI.dstColorBlendFactor = vk::BlendFactor::eOne;
            m_ColorBlendAttachmentCI.colorBlendOp        = vk::BlendOp::eAdd;
            m_ColorBlendAttachmentCI.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            m_ColorBlendAttachmentCI.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            m_ColorBlendAttachmentCI.alphaBlendOp        = vk::BlendOp::eAdd;
        }
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetColorAttachmentFormat(vk::Format format)
    {
        m_ColorFormat = format;
        m_RenderingCI.setColorAttachmentFormats({m_ColorFormat});
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetDepthAttachmentFormat(vk::Format format)
    {
        m_RenderingCI.setDepthAttachmentFormat(format);
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::DisableDepthTest()
    {
        m_DepthStencilCI = {
            .depthTestEnable       = VK_FALSE,
            .depthWriteEnable      = VK_FALSE,
            .depthCompareOp        = vk::CompareOp::eNever,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .front                 = {},
            .back                  = {},
            .minDepthBounds        = 0.0f,
            .maxDepthBounds        = 1.0f,
        };
        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::EnableDepthTest(bool write, vk::CompareOp compareOp)
    {
        m_DepthStencilCI = {
            .depthTestEnable       = VK_TRUE,
            .depthWriteEnable      = write ? VK_TRUE : VK_FALSE,
            .depthCompareOp        = compareOp,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .front                 = {},
            .back                  = {},
            .minDepthBounds        = 0.0f,
            .maxDepthBounds        = 1.0f,
        };

        return *this;
    }

    GraphicPipelineDesc& GraphicPipelineDesc::SetLayout(vk::PipelineLayout layout)
    {
        m_Layout = layout;
        return *this;
    }

    Pipeline::Pipeline(Device* device, const GraphicPipelineDesc& desc) : Resource(device), m_Desc(desc) {}

    Pipeline::~Pipeline() = default;

    void Pipeline::InitRHI()
    {
        vk::Device vkDevice = m_Device->GetDevice();

        vk::PipelineViewportStateCreateInfo viewportStateCI = {
            .viewportCount = 1,
            .pViewports    = nullptr,
            .scissorCount  = 1,
            .pScissors     = nullptr,
        };

        vk::PipelineColorBlendStateCreateInfo colorBlendingCI = {
            .logicOpEnable   = VK_FALSE,
            .logicOp         = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments    = &m_Desc.m_ColorBlendAttachmentCI,
        };

        vk::DynamicState                   dynamicStates[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicStateCI  = {
             .dynamicStateCount = 2,
             .pDynamicStates    = &dynamicStates[0],
        };

        vk::PipelineVertexInputStateCreateInfo vertexInputCI = {
            .vertexBindingDescriptionCount   = 0,
            .pVertexBindingDescriptions      = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions    = nullptr,
        };

        m_Layout = m_Desc.m_Layout;
        // transfer ownership of the layout to the pipeline

        vk::GraphicsPipelineCreateInfo pipelineCI;
        pipelineCI.setLayout(m_Layout)
            .setRenderPass(nullptr)
            .setStages(m_Desc.m_ShaderStagesCI)
            .setPVertexInputState(&vertexInputCI)
            .setPInputAssemblyState(&m_Desc.m_InputAssemblyCI)
            .setPViewportState(&viewportStateCI)
            .setPRasterizationState(&m_Desc.m_RasterizerCI)
            .setPMultisampleState(&m_Desc.m_MultisamplingCI)
            .setPDepthStencilState(&m_Desc.m_DepthStencilCI)
            .setPColorBlendState(&colorBlendingCI)
            .setPDynamicState(&dynamicStateCI)
            .setPNext(&m_Desc.m_RenderingCI);

        m_Pipeline = vkDevice.createGraphicsPipeline(nullptr, pipelineCI).value;
    }

    void Pipeline::ReleaseRHI()
    {
        vk::Device vkDevice = m_Device->GetDevice();
        vkDevice.destroyPipeline(m_Pipeline);
        vkDevice.destroyPipelineLayout(m_Layout);
    }
} // namespace wind