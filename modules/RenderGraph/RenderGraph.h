#pragma once

#include <vector>

#include "Backend/Stream.h"
#include "RenderGraphPass.h"

namespace wind
{
    class RenderGraph
    {
    public:
        RenderGraph()  = default;
        ~RenderGraph();

        void AddPass(RenderGraphPass* pass) { m_Passes.push_back(pass); }

        void Compile();
        void Execute(CommandStream* stream);

    private:
        std::vector<RenderGraphPass*> m_Passes;
    };
} // namespace wind