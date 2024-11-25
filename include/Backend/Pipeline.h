#pragma once

#include <span>

#include "Enum.h"
#include "Resource.h"
#include "Shader.h"
#include "VertexLayout.h"

namespace wind
{
    class Pipeline;

    class PipelineBuilder
    {
    public:
        PipelineBuilder(Device* device);

        PipelineBuilder& SetShaders(const std::span<Shader*>& shaders);
        PipelineBuilder& SetVertexLayout(VertexLayoutType vertexLayout);
        PipelineBuilder& SetPassType(PassType passType);
        PipelineBuilder& SetBlendMode(BlendMode blendMode);
        PipelineBuilder& SetLayout(vk::PipelineLayout layout);

        std::shared_ptr<Pipeline> Build();

        PipelineBuilder& ApplyDefaultState();

    private:
        struct NativePipeInfo
        {
            vk::GraphicsPipelineCreateInfo           pipelineCI;
            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCI;
            vk::PipelineRasterizationStateCreateInfo rasterizationStateCI;
            vk::PipelineColorBlendStateCreateInfo    colorBlendStateCI;
            vk::PipelineDepthStencilStateCreateInfo  depthStencilStateCI;
            vk::PipelineMultisampleStateCreateInfo   multisampleStateCI;
            vk::PipelineViewportStateCreateInfo      viewportStateCI;
            vk::PipelineDynamicStateCreateInfo       dynamicStateCI;
            vk::PipelineVertexInputStateCreateInfo   vertexInputStateCI;
            vk::PipelineRenderingCreateInfo          renderInfoKHR;

            std::vector<vk::PipelineShaderStageCreateInfo>     shaderStages;
            std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
            std::vector<vk::DynamicState>                      dynamicStates;
        };

        NativePipeInfo m_pipeInfo;
        Device*        m_device;
    };

    class Pipeline : public Resource
    {
    public:
        Pipeline(Device* device, const vk::GraphicsPipelineCreateInfo& createInfo);
        Pipeline(Device* device, const vk::ComputePipelineCreateInfo& createInfo);

        virtual ~Pipeline();

        auto GetNativePipeline() const { return m_pipeline; }

    private:
        friend class Device;

        PipelineType         m_type;
        PassType             m_passType;
        vk::Pipeline         m_pipeline;
        vk::Pipeline         m_pipelineCache;
        std::vector<Shader*> m_refShaders;
    };
} // namespace wind