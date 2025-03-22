#pragma once

#include "RenderGraph.h"

#include "Phase/RenderGraphPhase.h"

namespace wind
{
    RenderGraph::~RenderGraph()
    {
        
    }

    void RenderGraph::Compile()
    {
        // compile all passes
    }

    void RenderGraph::Execute()
    {
        for (auto& pass : m_Passes)
        {
            pass->Draw(m_Context.commandStream->GetCommandBuffer());
        }
    }
} // namespace wind