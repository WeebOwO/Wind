#include "GeometryPass.h"

#include "Graphics/GlobalRT.h"
#include "Graphics/View.h"
#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphBuilder.h"

namespace wind
{
    GeometryPass::GeometryPass(PipelineID id, PSOCache* psoCache) :
        m_PsoCacheLibrary(psoCache), m_PipelineID(id), RenderPassNode("GBufferPass")
    {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::Setup(RenderGraphBuilder& builder)
    {
        // setup the pass
        m_SceneColorHandle        = builder.GetResourceHandle(GlobalRT::BackBuffer);
        vk::ClearValue clearValue = {.color = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f})};

        std::vector<AttachmentInfo> renderTargets = 
        {
            {m_SceneColorHandle, clearValue, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore},
        };

        SetRenderTargets(renderTargets);
    }

    void GeometryPass::Execute(RenderGraphUpdateContext& context)
    {
        vk::CommandBuffer cmdBuffer = context.cmdBuffer;

        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);
    };
} // namespace wind