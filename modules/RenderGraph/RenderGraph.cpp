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

    void RenderGraph::Execute(CommandStream* stream)
    {
        for (auto& pass : m_Passes)
        {
            pass->Draw(stream->GetCommandBuffer());
        }
    }
} // namespace wind