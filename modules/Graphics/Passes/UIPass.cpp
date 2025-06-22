#include "UIPass.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "Graphics/GlobalRT.h"
#include "RenderGraph/RenderGraphBuilder.h"
#include "RenderGraph/RenderGraph.h"
#include "RenderGraph/RenderGraphResourceRegistry.h"

namespace wind
{
    void UIPass::Setup(RenderGraphBuilder& builder)
    {
        // setup the pass
        m_Data.backBuffer = builder.GetResourceHandle(GlobalRT::BackBuffer);
        vk::ClearValue clearValue = {.color = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f})};

        std::vector<AttachmentInfo> renderTargets = 
        {
            {m_Data.backBuffer, clearValue, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore},
        };

        SetRenderTargets(renderTargets);
    }

    void UIPass::Execute(RenderGraphUpdateContext& context)
    {
        vk::CommandBuffer cmdBuffer = context.commandStream->GetCommandBuffer();

        // Pipeline* pipeline = m_PsoCacheLibrary->GetPipeline(m_PipelineID);

        // cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetNativeHandle());

        // cmdBuffer.draw(3, 1, 0, 0);

        // draw UI here 
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmdBuffer);
    }
} // namespace wind