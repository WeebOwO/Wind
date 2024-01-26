#include "Command.h"

#include "Device.h"
#include "Engine/RenderConfig.h"

namespace wind
{
    CommandBuffer::CommandBuffer(GPUDevice& device, RenderCommandQueueType queueType) :
        RHIResource(device), m_queueType(queueType)
    {
        auto queueIndices = device.GetQueueIndices();
        auto vkDevice     = device.GetVkDeviceHandle();

        uint32_t queueIndex = queueType == RenderCommandQueueType::AsyncCompute ?
                                  queueIndices.asyncComputeQueueIndex.value() :
                                  queueIndices.mainQueueIndex.value();

        vk::CommandPoolCreateInfo poolCreateInfo {.queueFamilyIndex = queueIndex};

        m_cmdPool = vkDevice.createCommandPool(poolCreateInfo);

        vk::CommandBufferAllocateInfo allocateInfo {
            .commandPool        = m_cmdPool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1,
        };

        m_nativeHandle = vkDevice.allocateCommandBuffers(allocateInfo).front();
    }

    CommandBuffer::~CommandBuffer()
    {
        // wait command to finish job
        auto vkDevice = device.GetVkDeviceHandle();
        vkDevice.waitIdle();
        vkDevice.destroyCommandPool(m_cmdPool);
    }

    // reset the whole command pool, this is faster than reset the commandbuffer in multithread context
    // https://github.com/KhronosGroup/Vulkan-Samples/blob/main/samples/performance/command_buffer_usage/README.adoc
    void CommandBuffer::Reset() { device.GetVkDeviceHandle().resetCommandPool(m_cmdPool); }

    void CommandBuffer::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
        m_nativeHandle.begin(beginInfo);
    }

    vk::CommandBuffer CommandBuffer::Finish()
    {
        m_nativeHandle.end();
        return m_nativeHandle;
    }

    ImmCommandBuffer::ImmCommandBuffer(GPUDevice& device) : RHIResource(device)
    {
        m_handle = device.GetBackUpCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
        m_handle.begin(beginInfo);
    }

    void ImmCommandBuffer::PushTask(const TaskFunc& func) { m_tasks.push_back(func); }

    void ImmCommandBuffer::Submit()
    {
        for (const auto& func : m_tasks)
        {
            func(m_handle);
        }
        // this submit may cause gpu cpu stall
        device.SubmitBackUpCommandBuffer(m_handle);
    }

    void CommandBufferManager::Init(GPUDevice& device, uint32_t numThreads)
    {
        m_device                      = &device;
        const uint32_t totalPoolCount = numThreads * RenderConfig::MAX_FRAME_IN_FLIGHT;
        // create init command buffer
    }

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

    void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
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

    void CommandBuffer::ApplyTransition(const GPUTexture& texture, ResourceState newState) {}

    void CommandBuffer::RenderUI()
    {
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(main_draw_data, m_nativeHandle);
    }
} // namespace wind