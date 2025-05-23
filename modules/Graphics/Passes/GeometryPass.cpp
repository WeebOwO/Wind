#include "GeometryPass.h"

#include "Graphics/GlobalRT.h"
#include "Graphics/View.h"
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
        RenderGraphHandle handle = builder.GetResourceHandle(GlobalRT::SceneColor);
        vk::ClearValue clearValue = {.color = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f})};

        std::vector<AttachmentInfo> renderTargets = 
        {
            {handle, clearValue, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore},
        };

        SetRenderTargets(renderTargets);

        m_Descriptor.viewPort     = m_View->viewport;
        m_Descriptor.clearValue   = clearValue;
        m_Descriptor.renderArea   = vk::Rect2D {{0, 0}, m_View->viewport.width, m_View->viewport.height};
    }

    void GeometryPass::Execute(vk::CommandBuffer cmdBuffer)
    {
        BeginRendering(cmdBuffer);

        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);

        EndRendering(cmdBuffer);
    };
} // namespace wind