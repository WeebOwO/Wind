#pragma once

#include "RenderGraph.h"

#include "Phase/RenderGraphPhase.h"

namespace wind
{
    RenderGraph::RenderGraph()
        : m_FrameAllocator(new LinearAllocator(1024 * 1024)), // 1MB
          m_PersistentAllocator(new LinearAllocator(1024 * 1024)) // 1MB
    {
        m_FrameAllocator->Reset();
        m_PersistentAllocator->Reset();
    }

    RenderGraph::~RenderGraph()
    {
        
    }

    void RenderGraph::PrepareFrame(RenderGraphUpdateContext& context)
    {
        m_Context = context;
        m_FrameAllocator->Reset();
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
} // namespace wind