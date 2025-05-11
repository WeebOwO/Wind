#include "UIPass.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "Graphics/GlobalRT.h"
#include "RenderGraph/RenderGraphBuilder.h"

namespace wind
{
    void UIPass::Setup(RenderGraphBuilder& builder)
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
        m_Descriptor.renderArea   = vk::Rect2D {{0, 0}, m_View->viewport.width, m_View->viewport.height};
    }

    void UIPass::Execute(vk::CommandBuffer cmdBuffer)
    {
        BeginRendering(cmdBuffer);

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

        EndRendering(cmdBuffer);
    }
} // namespace wind