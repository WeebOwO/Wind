#pragma once

#include <string>

#include "BlackBoard.h"
#include "PassDesc.h"
#include "PassNode.h"
#include "RenderGraphID.h"
#include "RenderGraphPass.h"
#include "RenderGraphResource.h"
#include "RenderGraphTexture.h"
#include "ResourceNode.h"

namespace wind
{
    struct FrameData;
}

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

            uint32_t DeclareRenderPass(const std::string& name, const RenderPassDesc::Descriptor& desc);
            RenderGraphID<RenderGraphTexture> DeclareRenderPass(RenderGraphID<RenderGraphTexture> color,
                                                                uint32_t*                         index = nullptr);

            template<typename RESOURCE>
            RenderGraphID<RESOURCE> Create(const std::string& name, const typename RESOURCE::Descriptor& desc)
            {
                return m_renderGraph.Create<RESOURCE>(name, desc);
            }

        private:
            friend class RenderGraph;

            Builder(RenderGraph& renderGraph, PassNode* passNode) : m_renderGraph(renderGraph), m_passNode(passNode) {}

            RenderGraph& m_renderGraph;
            PassNode*    m_passNode;
        };

        RenderGraph(RenderGraphAllocator* allocator, FrameData* frameData);
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
        RenderGraphID<RESOURCE> Create(const std::string& name, const typename RESOURCE::Descriptor& desc)
        {
            VirutalResource* resource = new rg::Resource<RESOURCE>(name, desc);
            return RenderGraphID<RESOURCE>(AddResourceImpl(resource));
        }

        template<typename RESOURCE>
        RenderGraphID<RESOURCE>
        Import(const std::string& name, typename RESOURCE::Descriptor const& desc, const RESOURCE& resource)
        {
            VirutalResource* importedResource = new rg::ImportedResource<RESOURCE>(name, desc, resource);
            return RenderGraphID<RESOURCE>(AddResourceImpl(importedResource));
        }

        struct Empty
        {};

        template<typename Execute>
        void AddNoSetupPass(const std::string& name, Execute&& execute)
        {
            AddPass<Empty>(name, [](Builder& builder, Empty&) {}, std::forward<Execute>(execute));
        };

        auto& GetBlackBoard() { return m_blackBoard; }

        void Execute(CommandStream& commandStream);
        void Compile();

        // get resource from handle
        VirutalResource* GetResource(RenderGraphHandle handle) const;

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
        FrameData*                    m_frameData;
        BlackBoard                    m_blackBoard;
        std::vector<PassNode*>        m_passNodes;
        std::vector<ResourceNode>     m_resourceNodes;
        std::vector<ResourceSlot>     m_resourceSlots;
        std::vector<VirutalResource*> m_resources;
    };
} // namespace wind::rg