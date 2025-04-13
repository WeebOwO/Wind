#pragma once

#include <memory>

#include "Backend/Pipeline.h"
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

        void RecordRenderGrpah(RenderGraph& renderGraph) override;
        void Execute(vk::CommandBuffer cmdBuffer) override;

        void InitView(View* view);

        struct Data
        {
            RenderGraphHandle color;
        };

    private:
        View*                        m_View;
        Data                         m_Data;
        PSOCache*                    m_PsoCacheLibrary;
        PipelineID                   m_PipelineID;
        std::unordered_set<ShaderID> m_ShaderID;
        vk::RenderingInfo            m_RenderingInfo;
    };
} // namespace wind