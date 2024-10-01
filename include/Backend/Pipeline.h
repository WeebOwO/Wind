#pragma once

#include "Enum.h"
#include "Guard.h"

namespace wind
{
    class Pipeline
    {
    public:
        Pipeline()          = default;
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
        GraphicsPipeline()          = default;
        virtual ~GraphicsPipeline() = default;
    };

    class ComputePipeline : public Pipeline
    {
    public:
        ComputePipeline()          = default;
        virtual ~ComputePipeline() = default;
    };
} // namespace wind