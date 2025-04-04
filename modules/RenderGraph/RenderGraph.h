#pragma once

#include <any>
#include <vector>

#include "Backend/Stream.h"
#include "Core/Allocator.h"
#include "RenderGraphPass.h"

namespace wind
{
    class RenderGraphPhase;

    struct RenderGraphUpdateContext
    {
        uint32_t       frameIndex;
        CommandStream* commandStream;
    };

    class RenderGraph
    {
    public:
        struct Builder
        {
            Builder(RenderGraph& renderGraph, RenderGraphPass* currentPass)
                : m_RenderGraph(renderGraph), m_CurrentPass(currentPass) {}
            
        private:
            RenderGraph&     m_RenderGraph;
            RenderGraphPass* m_CurrentPass;
        };

        RenderGraph();
        ~RenderGraph();

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> 
        void AddPhase(Args&&... args);

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPass> 
        Builder AddPass(Args&&... args);

        void Compile();
        void Execute();

        void PrepareFrame(RenderGraphUpdateContext& context);

    private:
        friend class RenderGraphBuilder;

        RenderGraphUpdateContext m_Context;
        LinearAllocator*         m_FrameAllocator;
        LinearAllocator*         m_PersistentAllocator;

        std::vector<RenderGraphPass*>  m_Passes;
        std::vector<RenderGraphPhase*> m_Phases;
    };

} // namespace wind

#include "RenderGraph.inl"