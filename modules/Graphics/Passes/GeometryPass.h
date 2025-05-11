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
        void Execute(vk::CommandBuffer cmdBuffer) override;

    private:
        RenderGraphHandle            m_SceneColorHandle;
        PSOCache*                    m_PsoCacheLibrary;
        PipelineID                   m_PipelineID;
        std::unordered_set<ShaderID> m_ShaderID;
    };
} // namespace wind