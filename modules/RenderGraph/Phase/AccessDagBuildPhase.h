#pragma once

#include "RenderGraphPhase.h"

namespace wind
{
    class AccessDagBuildPhase : public RenderGraphPhase
    {
    public:
        AccessDagBuildPhase(RenderGraph& renderGraph) : RenderGraphPhase(renderGraph) {}

        void Run(RenderGraphUpdateContext& context) override;
    };
} // namespace wind