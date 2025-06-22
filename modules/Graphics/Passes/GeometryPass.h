#pragma once

#include "Graphics/PSOCache.h"
#include "Backend/Pipeline.h"
#include "RenderGraph/RenderGraphPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;
    class View;

    class GeometryPass : public RenderPassNode
    {
    public:
        GeometryPass(PipelineManager* pipelineManager);
        ~GeometryPass() override;

        void Setup(RenderGraphBuilder& renderGraph) override;
        void Execute(RenderGraphUpdateContext& context) override;

        void BakeRenderPipeline() override;

    private:
        RenderGraphHandle            m_SceneColorHandle;
        PipelineManager*             m_PipelineManager;
        CustomPipelineID             m_CustomPipelineID;
    };
} // namespace wind