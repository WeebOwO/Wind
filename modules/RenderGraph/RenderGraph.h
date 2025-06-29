#pragma once

#include <unordered_map>
#include <vector>

#include "Blackboard.h"
#include "Core/Allocator.h"
#include "RenderGraphHandle.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "Scene/GPUScene.h"
#include "VirtualResource.h"


namespace wind
{
    class RenderGraphPhase;
    class RenderGraphResourceRegistry;

    struct RenderGraphUpdateContext
    {
        uint32_t                     frameIndex;
        CommandStream*               commandStream;
        RenderGraphResourceRegistry* resourceRegistry;
        GPUScene*                    scene;
    };

    class RenderGraph
    {
    public:
        RenderGraph(Device* device);
        ~RenderGraph();

        auto& GetBlackboard() { return m_Blackboard; }
        template<typename T, typename... Args>
        requires std::derived_from<T, RenderGraphPhase> void AddPhase(Args&&... args);

        template<typename T>
        requires std::derived_from<T, PassNode> void AddPass(T* pass);

        RenderGraphHandle AllocRenderGraphResource(const RDGResourceDesc& resourceDesc);
        RenderGraphHandle ImportRenderGraphResource(VirtualResource* resource);

        void Execute();
        void PrepareFrame(RenderGraphUpdateContext& context);

        VirtualResource* GetResource(RenderGraphHandle handle)
        {
            if (handle.isInitialized())
            {
                return m_Resources[handle.index];
            }
            return nullptr;
        }

        void SetView(View* view) { m_View = view; }

    private:
        template<typename T>
        using Scoped = std::unique_ptr<T>;

        friend class RenderGraphBuilder;
        friend class RenderGraphResourceRegistry;

        void BeforeExecute(PassNode* pass);
        void AfterExecute(PassNode* pass);

        void AddPassInternal(PassNode* pass);

        RenderGraphHandle WriteInternal(PassNode* pass, RenderGraphHandle handle);

        void Compile();

        struct ResourceTransition
        {
            RenderGraphHandle   handle;
            ResourceState       from;
            ResourceState       to;
            RDGSubresourceRange range;
        };

        void CollectTransitions(PassNode* pass, std::vector<ResourceTransition>& transitions);
        void InsertBarriers(vk::CommandBuffer cmdBuffer, const std::vector<ResourceTransition>& transitions);

        RenderGraphUpdateContext            m_Context;
        Scoped<RenderGraphResourceRegistry> m_ResourceRegistry;
        Scoped<LinearAllocator>             m_FrameAllocator;
        Scoped<LinearAllocator>             m_PersistentAllocator;
        std::vector<VirtualResource*>       m_Resources;
        std::vector<PassNode*>              m_Passes;
        std::vector<ResourceNode*>          m_ResourceNodes;
        std::vector<RenderGraphPhase*>      m_Phases;
        Blackboard                          m_Blackboard;
        Device*                             m_Device;
        View*                               m_View;
        // global resource states tracking
        std::unordered_map<RenderGraphHandle, ResourceState> m_GlobalResourceStates;
    };

} // namespace wind

#include "RenderGraph.inl"