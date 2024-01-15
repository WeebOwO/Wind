#include "Backend/Texture.h"
#include "Command.h"

// imgui incldue part
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"

#include "Texture.h"

namespace wind
{
    void CommandBuffer::BeginRenderPass(const vk::RenderPassBeginInfo& renderPassBeginInfo)
    {
        m_nativeHandle.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    }

    void CommandBuffer::EndRenderPass() { m_nativeHandle.endRenderPass(); }

    void CommandBuffer::BeginRendering(const vk::RenderingInfo& renderingInfo)
    {
        m_nativeHandle.beginRendering(renderingInfo);
    }

    void CommandBuffer::BindPSO(const vk::Pipeline& pipeline)
    {
        m_nativeHandle.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }

    void CommandBuffer::EndRendering() { m_nativeHandle.endRendering(); }

    void
    CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        m_nativeHandle.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandBuffer::DrawIndexed(uint32_t indexCount,
                                     uint32_t instanceCount,
                                     uint32_t firstIndex,
                                     uint32_t vertexOffset,
                                     uint32_t firstInstance)
    {
        m_nativeHandle.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandBuffer::BindVertexBuffer(uint32_t          firstBinding,
                                          uint32_t          bindingCount,
                                          const vk::Buffer& buffer,
                                          vk::DeviceSize    offset)
    {
        m_nativeHandle.bindVertexBuffers(firstBinding, bindingCount, &buffer, &offset);
    }

    void CommandBuffer::BindIndexBuffer(const vk::Buffer& buffer, vk::DeviceSize offset, vk::IndexType indexType)
    {
        m_nativeHandle.bindIndexBuffer(buffer, offset, indexType);
    }

    void CommandBuffer::SetViewport(const vk::Viewport& viewport) { m_nativeHandle.setViewport(0, 1, &viewport); }

    void CommandBuffer::SetScissor(int offsetx, int offsety, uint32_t width, uint32_t height)
    {
        vk::Rect2D scissor {.offset = {.x = offsetx, .y = offsety}, .extent = {.width = width, .height = height}};

        m_nativeHandle.setScissor(0, 1, &scissor);
    }

    void CommandBuffer::TransferImageLayout(const vk::Image&                 image,
                                             vk::AccessFlags                  srcMask,
                                             vk::AccessFlags                  dstMask,
                                             vk::ImageLayout                  oldlayout,
                                             vk::ImageLayout                  newLayout,
                                             vk::PipelineStageFlags           srcFlags,
                                             vk::PipelineStageFlags           dstFlags,
                                             const vk::ImageSubresourceRange& subRange)
    {

        vk::ImageMemoryBarrier imageBarrier {
            .srcAccessMask       = srcMask,
            .dstAccessMask       = dstMask,
            .oldLayout           = oldlayout,
            .newLayout           = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = image,
            .subresourceRange    = subRange,
        };

        m_nativeHandle.pipelineBarrier(srcFlags, dstFlags, {}, 0, nullptr, 0, nullptr, 1, &imageBarrier);
    }

    void CommandBuffer::TransferImageLayout(const GPUTexture& texture, vk::ImageLayout newLayout) 
    {
        
    }

    void CommandBuffer::RenderUI()
    {
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(main_draw_data, m_nativeHandle);
    }

} // namespace wind