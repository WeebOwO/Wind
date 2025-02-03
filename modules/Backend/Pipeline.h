#pragma once

#include <variant>
#include <vector>

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    class Device;
    class Pipeline;

    enum class BlendMode
    {
        None,
        AlphaBlend,
        Additive,
    };

    struct GraphicPipelineDesc
    {
    public:
        GraphicPipelineDesc();

        GraphicPipelineDesc& SetShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader);
        GraphicPipelineDesc& SetLayoutInfo(vk::PipelineLayoutCreateInfo layout);
        GraphicPipelineDesc& SetInputTopology(vk::PrimitiveTopology topology);
        GraphicPipelineDesc& SetPolygonMode(vk::PolygonMode mode);
        GraphicPipelineDesc& SetCullMode(vk::CullModeFlags mode, vk::FrontFace frontFace);
        GraphicPipelineDesc& SetMultisamplingNone();
        GraphicPipelineDesc& SetBlendMode(BlendMode mode);

        GraphicPipelineDesc& SetColorAttachmentFormat(vk::Format format);
        GraphicPipelineDesc& SetDepthAttachmentFormat(vk::Format format);
        GraphicPipelineDesc& DisableDepthTest();
        GraphicPipelineDesc& EnableDepthTest(bool write, vk::CompareOp compareOp);

    private:
        friend class Device;
        friend class Pipeline;

        void Clear();

        std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStagesCI;
        vk::PipelineInputAssemblyStateCreateInfo       m_InputAssemblyCI;
        vk::PipelineRasterizationStateCreateInfo       m_RasterizerCI;
        vk::PipelineColorBlendAttachmentState          m_ColorBlendAttachmentCI;
        vk::PipelineMultisampleStateCreateInfo         m_MultisamplingCI;
        vk::PipelineDepthStencilStateCreateInfo        m_DepthStencilCI;
        vk::PipelineRenderingCreateInfo                m_RenderingCI;
        vk::Format                                     m_ColorFormat;
        // need to call default constructor to initialize the layout info
        vk::PipelineLayoutCreateInfo m_LayoutCI;
    };

    class Pipeline : public Resource
    {
    public:
        Pipeline(Device* device, const GraphicPipelineDesc& desc);
        ~Pipeline() override;

        vk::Pipeline GetNativeHandle() const { return m_Pipeline; }

        void InitRHI() override;
        void ReleaseRHI() override;

    private:
        GraphicPipelineDesc m_Desc;
        vk::Pipeline        m_Pipeline;
        vk::PipelineLayout  m_Layout;
    };
} // namespace wind