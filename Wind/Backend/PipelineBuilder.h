#pragma once

#include "std.h"

#include "RasterShader.h"
#include "VulkanHeader.h"

namespace wind
{
    class RasterShader;

    enum EVertexType
    {
        NoVertex,
        StaticMesh
    };

    struct DepthState
    {
        bool          depthTest;
        bool          detphWrite;
        vk::CompareOp compareOP;
    };

    struct StencilState
    {
        bool stencilTest;
    };

    struct BlendState
    {
        bool    blendEnable;
        uint8_t mrtCount = 1;
    };

    struct RenderState
    {
        DepthState    depthState;
        StencilState  setencilState;
        BlendState    blendState;
        RasterShader* rasterShader;
    };

    // todo : add more options for different shader
    class PipelineBuilder
    {
    public:
        static constexpr vk::ColorComponentFlags BLEND_ALL_FLAG =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA;

        PipelineBuilder& SetInputAssemblyState(vk::PrimitiveTopology topology, bool primitiveRestartEnable);
        PipelineBuilder& SetVertexType(EVertexType vertexType); // will change this in future
        PipelineBuilder& SetRasterizationState(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace);

        PipelineBuilder& SetRenderState(const RenderState& renderState); // this part care about blend

        vk::Pipeline Build(const vk::Device device);

    private:
        void SetShaderState(const RasterShader& shader);

        vk::PipelineInputAssemblyStateCreateInfo m_inputAssemblyStateInfo {};
        vk::PipelineVertexInputStateCreateInfo   m_inputStateCreateInfo {};
        vk::PipelineRasterizationStateCreateInfo m_rasterizationStateInfo {};
        vk::PipelineColorBlendStateCreateInfo    m_blendStateInfo {};
        vk::PipelineDepthStencilStateCreateInfo  m_depthStencilState {};
        vk::PipelineLayout                       m_layout;

        std::vector<vk::PipelineColorBlendAttachmentState> m_attachmentState;
        std::array<vk::PipelineShaderStageCreateInfo, 2>   m_shaderStages;

        std::vector<vk::VertexInputBindingDescription>   m_inputBindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> m_inputAttributeDescriptions;
    };
} // namespace wind