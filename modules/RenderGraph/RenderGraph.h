#pragma once

#include <vector>

#include "Blackboard.h"
#include "RenderGraphPass.h"
#include "RenderGraphHandle.h"
#include "RenderGraphResource.h"
#include "VirtualResource.h"
#include "Core/Allocator.h"
#include "Backend/Stream.h"

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
            Builder(RenderGraph& renderGraph, PassNode* currentPass) :
                m_RenderGraph(renderGraph), m_CurrentPass(currentPass)
            {
                currentPass->RegisterGraph(&renderGraph);
            }

            RenderGraphHandle AllocRenderGraphResource(const RDGResourceDesc& resourceDesc);
            RenderGraphHandle ImportRenderGraphResource(VirtualResource* resource);

            template<typename Setup>
            void Invoke(Setup setup)
            {
                if (m_CurrentPass != nullptr)
                {
                    setup(m_CurrentPass);
                }
            }

        private:
            RenderGraph& m_RenderGraph;
            PassNode*    m_CurrentPass;
        };

        RenderGraph();
        ~RenderGraph();

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> 
        void AddPhase(Args&&... args);

        template<typename T>
        requires std::derived_from<T, PassNode>
        Builder AddPass(T* pass);

        RenderGraphHandle AllocRenderGraphResource(const RDGResourceDesc& resourceDesc);
        RenderGraphHandle ImportRenderGraphResource(VirtualResource* resource);

        void Compile();
        void Execute();

        void PrepareFrame(RenderGraphUpdateContext& context);

        auto& GetBlackboard() { return m_Blackboard; }

        VirtualResource* GetResource(RenderGraphHandle handle)
        {
            if (handle.isInitialized()) 
            {
                return m_Resources[handle.index];
            }
            return nullptr;
        }

    private:
        friend class RenderGraphBuilder;

        RenderGraphUpdateContext m_Context;
        LinearAllocator*         m_FrameAllocator;
        LinearAllocator*         m_PersistentAllocator;

        std::vector<VirtualResource*>  m_Resources;
        std::vector<PassNode*>         m_Passes;
        std::vector<RenderGraphPhase*> m_Phases;
        Blackboard                     m_Blackboard;
    };

} // namespace wind

#include "RenderGraph.inl"