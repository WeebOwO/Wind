#include "RenderGraphBuilder.h"

#include "RenderGraph.h"

namespace wind 
{
    RenderGraphBuilder::RenderGraphBuilder(RenderGraph& renderGraph, PassNode* currentPass) :
        m_RenderGraph(renderGraph), m_Pass(currentPass)
    {
        // constructor
        m_Pass->RegisterGraph(&m_RenderGraph);
    }

    RenderGraphHandle RenderGraphBuilder::Create(const RDGResourceDesc& resourceDesc)
    {
        // create a new resource
        return m_RenderGraph.AllocRenderGraphResource(resourceDesc);
    }

    RenderGraphHandle RenderGraphBuilder::Import(VirtualResource* resource, const std::string& name)
    {
        // import a resource
        resource->name = name;
        resource->imported = true;
        return m_RenderGraph.ImportRenderGraphResource(resource);
    }

    RenderGraphHandle RenderGraphBuilder::GetResourceHandle(const std::string& name)
    {
        // get a resource handle by name
        return m_RenderGraph.GetBlackboard().Get(name);
    }
}