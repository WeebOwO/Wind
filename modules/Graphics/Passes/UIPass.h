#pragma once

#include "RenderGraph/RenderGraphPass.h"
#include "Scene/Viewport.h"

namespace wind
{
    // draw at backbuffer directly
    class UIPass : public RenderPassNode
    {
    public:
        UIPass() : RenderPassNode("UIPass") {};

        void Setup(RenderGraphBuilder& renderGraph) override;
        void Execute(RenderGraphUpdateContext& context) override;

        void SetViewport(const Viewport& viewport);

    private:
        Viewport m_Viewport;
    };
} // namespace wind