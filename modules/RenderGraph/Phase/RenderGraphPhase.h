#pragma once

namespace wind
{
    class RenderGraph;
    struct RenderGraphUpdateContext;

    class RenderGraphPhase
    {
    public:
        RenderGraphPhase(RenderGraph& renderGraph) : m_RenderGraph(renderGraph) {}
        virtual ~RenderGraphPhase() = default;

        virtual void Run(RenderGraphUpdateContext& context) = 0;

    private:
        RenderGraph& m_RenderGraph;
    };
} // namespace wind