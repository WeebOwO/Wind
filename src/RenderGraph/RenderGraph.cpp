#include "RenderGraph/RenderGraph.h"

#include "Graphics/Renderer.h"
#include "RenderGraph/PassNode.h"
#include "RenderGraph/RenderGraphID.h"

namespace wind::rg
{
    RenderGraph::RenderGraph(RenderGraphAllocator* allocator, FrameData* frameData) : m_allocator(allocator) {}

    RenderGraph::~RenderGraph()
    {
        for (auto* node : m_passNodes)
        {
            delete node;
        }

        for (auto* resource : m_resources)
        {
            delete resource;
        }
    }

    RenderGraph::Builder RenderGraph::AddPassImpl(const std::string& name, RenderGraphPassBase* pass)
    {
        auto* node = new RenderPassNode(this, name, pass);
        m_passNodes.push_back(node);
        pass->SetNode(node);
        return Builder(*this, node);
    }

    uint32_t RenderGraph::Builder::DeclareRenderPass(const std::string& name, const RenderPassDesc::Descriptor& desc)
    {
        RenderPassNode* node = static_cast<RenderPassNode*>(m_passNode);
        return node->DeclareRenderPass(name, desc);
    }

    RenderGraphHandle RenderGraph::AddResourceImpl(VirutalResource* resource)
    {
        RenderGraphHandle handle(m_resources.size());
        m_resources.push_back(resource);
        return handle;
    }

    void RenderGraph::Compile()
    {
        // TODO: compile the render graph into topological order
    }

    void RenderGraph::Execute(Device& device)
    {
        // execute the render graph
        auto stream = device.CreateResource<CommandStream>(RenderCommandQueueType::Graphics);

        for (auto passNode : m_passNodes)
        {
            ResourceRegistry registry(*this, *passNode);
            passNode->Execute(registry, *stream);
        }

        return;
    }

    VirutalResource* RenderGraph::GetResource(RenderGraphHandle handle) const { return m_resources[handle.index]; }
} // namespace wind::rg