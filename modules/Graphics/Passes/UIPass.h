#pragma once

#include "Scene/Viewport.h"
#include "Graphics/PSOCache.h"
#include "RenderGraph/RenderGraphPass.h"

namespace wind
{
    // draw at backbuffer directly
    class UIPass : public RenderPassNode
    {
    public:
        struct Data 
        {
            RenderGraphHandle backBuffer;
        };

        UIPass(PipelineManager* pipelineManager) : RenderPassNode("UIPass"), m_PipelineManager(pipelineManager) {};

        void Setup(RenderGraphBuilder& renderGraph) override;
        void Execute(RenderGraphUpdateContext& context) override;

        void SetViewport(const Viewport& viewport);

    private:
        Data m_Data;
        PipelineID m_PipelineID;
        PipelineManager* m_PipelineManager;
    };
} // namespace wind