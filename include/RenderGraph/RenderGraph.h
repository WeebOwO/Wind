#pragma once

#include "ResourceNode.h"
#include <string>

#include "BlackBoard.h"
#include "PassDesc.h"
#include "PassNode.h"
#include "RenderGraphID.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "RenderGraphTexture.h"

namespace wind::rg
{
    class RenderGraph;
    class RenderGraphAllocator;
    class RenderGraphPassExecutor;

    class RenderGraph
    {
    public:
        struct Builder
        {
            Builder(const Builder&)            = delete;
            Builder& operator=(const Builder&) = delete;

            uint32_t                          DeclareRenderPass(const std::string& name, const RenderPassDesc& desc);
            RenderGraphID<RenderGraphTexture> DeclareRenderPass(RenderGraphID<RenderGraphTexture> color,
                                                                uint32_t*                         index = nullptr);

            template<typename RESOURCE>
            RenderGraphID<RESOURCE> Create(const std::string& name, const RESOURCE::Descriptor& desc)
            {
                return m_renderGraph.Create<RESOURCE>(name, desc);
            }

        private:
            friend class RenderGraph;

            Builder(RenderGraph& renderGraph, PassNode* passNode) : m_renderGraph(renderGraph), m_passNode(passNode) {}

            RenderGraph& m_renderGraph;
            PassNode*    m_passNode;
        };

        RenderGraph(RenderGraphAllocator* allocator);
        ~RenderGraph();

        // add pass interfaces
        template<typename Data, typename Setup, typename Execute>
        RenderGraphPass<Data>& AddPass(const std::string& name, Setup&& setup, Execute&& execute)
        {
            auto*   pass    = new RenderGraphPassConcrete<Data, Execute>(std::forward<Execute>(execute));
            Builder builder = AddPassImpl(name, pass);
            setup(builder, const_cast<Data&>(pass->GetData()));
            return *pass;
        }

        // resource create interfaces
        template<typename RESOURCE>
        RenderGraphID<RESOURCE> Create(const std::string& name, const RESOURCE::Descriptor& desc)
        {
            VirutalResource* resource = new rg::Resource<RESOURCE>(name, desc);
            return RenderGraphID<RESOURCE>(AddResourceImpl(resource));
        }

        auto& GetBlackBoard() { return m_blackBoard; }

        void         Execute();
        RenderGraph& Compile();

    private:
        Builder           AddPassImpl(const std::string& name, RenderGraphPassBase* pass);
        RenderGraphHandle AddResourceImpl(VirutalResource* resource);

        struct ResourceSlot
        {
            using Version   = RenderGraphHandle::Version;
            using Index     = int16_t;
            Index   rid     = 0;  // VirtualResource* index in mResources
            Index   nid     = 0;  // ResourceNode* index in mResourceNodes
            Index   sid     = -1; // ResourceNode* index in mResourceNodes for reading subresource's parent
            Version version = 0;
        };

        RenderGraphAllocator*         m_allocator;
        BlackBoard                    m_blackBoard;
        std::vector<PassNode*>        m_passNodes;
        std::vector<ResourceNode>     m_resourceNodes;
        std::vector<ResourceSlot>     m_resourceSlots;
        std::vector<VirutalResource*> m_resources;
    };
} // namespace wind::rg