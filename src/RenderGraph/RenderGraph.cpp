#include "RenderGraph/RenderGraph.h"

#include "Graphics/Renderer.h"
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

    void RenderGraph::Execute() 
    {
        
    }
} // namespace wind::rg