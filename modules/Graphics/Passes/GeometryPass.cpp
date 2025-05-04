#include "GeometryPass.h"

#include "Graphics/GlobalRT.h"
#include "Graphics/View.h"
#include "RenderGraph/RenderGraph.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

namespace wind
{
    GeometryPass::GeometryPass(PipelineID id, PSOCache* psoCache) :
        m_PsoCacheLibrary(psoCache), m_PipelineID(id), RenderPassNode("GBufferPass")
    {}

    GeometryPass::~GeometryPass() {}

    void GeometryPass::InitView(View* view) { m_View = view; }

    void GeometryPass::RecordRenderGrpah(RenderGraph& renderGraph)
    {
        // setup the pass
        auto  builder    = renderGraph.AddPass<GeometryPass>(this);
        auto& blackboard = renderGraph.GetBlackboard();

        m_Descriptor.attachments.color[0] = blackboard.Get(GlobalRT::BackBuffer);
        vk::ClearValue clearValue         = {};
        clearValue.color                  = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f});
        m_Descriptor.viewPort             = m_View->viewport;
        m_Descriptor.clearValue           = clearValue;
        m_Descriptor.renderArea           = vk::Rect2D {{0, 0}, m_View->viewport.width, m_View->viewport.height};
    }

    void GeometryPass::Execute(vk::CommandBuffer cmdBuffer)
    {
        BeforeRendering(cmdBuffer);

        Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());
        cmdBuffer.draw(3, 1, 0, 0);

        ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmdBuffer);

        AfterRendering(cmdBuffer);
    };
} // namespace wind