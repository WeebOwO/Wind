#pragma once

#include "RenderGraph.h"

#include "Phase/RenderGraphPhase.h"

namespace wind
{
    RenderGraphHandle RenderGraph::Builder::AllocRenderGraphResource(const RDGResourceDesc& resourceDesc)
    {
        return m_RenderGraph.AllocRenderGraphResource(resourceDesc);
    }

    RenderGraph::RenderGraph() :
        m_FrameAllocator(new LinearAllocator(1024 * 1024)),     // 1MB
        m_PersistentAllocator(new LinearAllocator(1024 * 1024)) // 1MB
    {
        m_FrameAllocator->Reset();
        m_PersistentAllocator->Reset();
    }

    RenderGraph::~RenderGraph() {}

    void RenderGraph::PrepareFrame(RenderGraphUpdateContext& context)
    {
        m_Context = context;
        m_FrameAllocator->Reset();
        m_Resources.clear();
        m_Passes.clear();
    }

    void RenderGraph::Compile()
    {
        // compile all passes
    }

    void RenderGraph::Execute()
    {
        for (auto& pass : m_Passes)
        {
            pass->Execute(m_Context.commandStream->GetCommandBuffer());
        }
    }

    RenderGraphHandle RenderGraph::AllocRenderGraphResource(const RDGResourceDesc& resourceDesc)
    {
        // allocate a new resource
        VirtualResource* resource;
        m_Resources.push_back(resource);
        return RenderGraphHandle(m_Resources.size() - 1);
    }

    RenderGraphHandle RenderGraph::ImportRenderGraphResource(VirtualResource* resource)
    {
        // import a resource
        m_Resources.push_back(resource);
        return RenderGraphHandle(m_Resources.size() - 1);
    }
} // namespace wind