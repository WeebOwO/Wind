#pragma once

#include <memory>

#include "Backend/Pipeline.h"
#include "Graphics/PSOCache.h"
#include "RenderGraph/RenderGraphPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;

    class GeometryPass : public RenderGraphPass
    {
    public:
        GeometryPass(PipelineID pipeline, PSOCache* globalPSOCache);
        ~GeometryPass() override;

        void RecordRenderGrpah(RenderGraph& renderGraph) override;
        void Execute(vk::CommandBuffer cmdBuffer) override;

    private:
        PSOCache*                    m_PsoCacheLibrary;
        PipelineID                   m_PipelineID;
        std::unordered_set<ShaderID> m_ShaderID;
        vk::RenderingInfo            m_RenderingInfo;
    };
} // namespace wind