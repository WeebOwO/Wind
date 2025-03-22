#pragma once

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
        RenderGraph(LinearAllocator* allocator) : m_Allocator(allocator) {}
        ~RenderGraph();

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> 
        void AddPhase(Args&&... args);

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPass> 
        void AddPass(Args&&... args);

        void Compile();
        void Execute();

        void PrepareFrame(RenderGraphUpdateContext& context) { m_Context = context; }

    private:
        friend class RenderGraphBuilder;

        RenderGraphUpdateContext m_Context;
        LinearAllocator*         m_Allocator;

        std::vector<RenderGraphPass*>  m_Passes;
        std::vector<RenderGraphPhase*> m_Phases;
    };

} // namespace wind

#include "RenderGraph.inl"