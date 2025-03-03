#pragma once

#include <vector>

#include "Backend/Stream.h"
#include "RenderGraphPass.h"

namespace wind
{
    class RenderGraphPhase;

    struct RenderGraphUpdateContext
    {};

    class RenderGraph
    {
    public:
        RenderGraph() = default;
        ~RenderGraph();

        void AddPass(RenderGraphPass* pass) { m_Passes.push_back(pass); }
        void AddPhase(RenderGraphPhase* phase) { m_Phases.push_back(phase); }

        void Compile();
        void Execute(CommandStream* stream);

    private:
        friend class RenderGraphBuilder;

        std::vector<RenderGraphPass*>  m_Passes;
        std::vector<RenderGraphPhase*> m_Phases;
    };
} // namespace wind