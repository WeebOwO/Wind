#pragma once

#include <vector>

#include "Backend/Stream.h"
#include "Blackboard.h"
#include "Core/Allocator.h"
#include "RenderGraphHandle.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "VirtualResource.h"

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
        RenderGraph(Device* device);
        ~RenderGraph();

        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> void AddPhase(Args&&... args);

        template<typename T>
        requires std::derived_from<T, PassNode> void AddPass(T* pass);

        RenderGraphHandle AllocRenderGraphResource(const RDGResourceDesc& resourceDesc);
        RenderGraphHandle ImportRenderGraphResource(VirtualResource* resource);

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

        void BeforeExecute(PassNode* pass);
        void AfterExecute(PassNode* pass);

        void AddPassInternal(PassNode* pass);

        RenderGraphHandle WriteInternal(PassNode* pass, RenderGraphHandle handle);

        void Compile();

        RenderGraphUpdateContext       m_Context;
        LinearAllocator*               m_FrameAllocator;
        LinearAllocator*               m_PersistentAllocator;
        std::vector<VirtualResource*>  m_Resources;
        std::vector<PassNode*>         m_Passes;
        std::vector<ResourceNode*>     m_ResourceNodes;
        std::vector<RenderGraphPhase*> m_Phases;
        Blackboard                     m_Blackboard;
        Device*                        m_Device;
    };

} // namespace wind

#include "RenderGraph.inl"