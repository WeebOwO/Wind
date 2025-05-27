#pragma once

#include "Graphics/PSOCache.h"
#include "RenderGraph/RenderGraphPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;
    class View;

    class GeometryPass : public RenderPassNode
    {
    public:
        GeometryPass(PipelineID pipeline, PSOCache* globalPSOCache);
        ~GeometryPass() override;

        void Setup(RenderGraphBuilder& renderGraph) override;
        void Execute(RenderGraphUpdateContext& context) override;

    private:
        void BlitToBackBuffer(vk::CommandBuffer cmdBuffer, vk::Image color, vk::Image backBuffer);

        RenderGraphHandle            m_SceneColorHandle;
        PSOCache*                    m_PsoCacheLibrary;
        PipelineID                   m_PipelineID;
        std::unordered_set<ShaderID> m_ShaderID;
    };
} // namespace wind