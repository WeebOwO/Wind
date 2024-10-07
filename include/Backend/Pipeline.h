#pragma once

#include "Enum.h"

#include "Resource.h"
#include "Shader.h"

namespace wind
{
    class Pipeline : public Resource
    {
    public:
        Pipeline(Device* device, PipelineType type) : Resource(device, Tag::Pipeline), m_type(type) {}
        virtual ~Pipeline() = default;

    protected:
        PipelineType       m_type;
        vk::Pipeline       m_pipeline;
        vk::PipelineLayout m_pipelineLayout;
        vk::Pipeline       m_pipelineCache;
    };

    class GraphicsPipeline : public Pipeline
    {
    public:
        GraphicsPipeline(Device* device, RasterShader* shader) :
            Pipeline(device, PipelineType::Graphics), m_shader(shader)
        {}

        virtual ~GraphicsPipeline() = default;

        void Init() override;
        void Destroy() override;

    private:
        // pipeline info
        vk::GraphicsPipelineCreateInfo                 m_createInfo;
        std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
        vk::PipelineDynamicStateCreateInfo             m_dynamicState;
        vk::PipelineVertexInputStateCreateInfo         m_vertexInputState;
        vk::PipelineInputAssemblyStateCreateInfo       m_inputAssemblyState;
        vk::PipelineRasterizationStateCreateInfo       m_rasterizationState;
        vk::PipelineMultisampleStateCreateInfo         m_multisampleState;
        vk::PipelineDepthStencilStateCreateInfo        m_depthStencilState;
        vk::PipelineColorBlendStateCreateInfo          m_colorBlendState;
        vk::PipelineLayoutCreateInfo                   m_layoutInfo;

        RasterShader* m_shader;
    };
} // namespace wind