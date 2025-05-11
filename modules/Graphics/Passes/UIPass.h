#pragma once

#include "RenderGraph/RenderGraphPass.h"

namespace wind
{
    // draw at backbuffer directly
    class UIPass : public RenderPassNode
    {
    public:
        UIPass() : RenderPassNode("UIPass") {};

        void Setup(RenderGraphBuilder& renderGraph) override;
        void Execute(vk::CommandBuffer cmdBuffer) override;

    private:
        
    };
} // namespace wind