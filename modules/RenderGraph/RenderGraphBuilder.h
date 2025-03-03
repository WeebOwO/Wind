#pragma once

#include "Backend/Stream.h"
#include "Core/Allocator.h"
#include "Phase/RenderGraphPhase.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"

namespace wind
{
    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(CommandStream* stream, LinearAllocator* allocator) :
            m_CommandStream(stream), m_Allocator(allocator)
        {}

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> void AddPhase(Args&&... args);

        void AddPass(RenderGraphPass* pass);
        void Execute();

    private:
        LinearAllocator* m_Allocator;
        CommandStream*   m_CommandStream;
        RenderGraph      m_RenderGraph;
        uint32_t         m_memoryOffset = 0;
    };
} // namespace wind

#include "RenderGraphBuilder.inl"