#pragma once

#include "Backend/Stream.h"
#include "Backend/Device.h"

namespace wind
{
    CommandStream::CommandStream(Device* device, RenderCommandQueueType queueType, StreamMode mode) :
        Resource(device, Tag::CommandStream), m_StreamMode(mode), m_QueueType(queueType)
    {
        Init();
    }

    CommandStream::~CommandStream() noexcept { Destroy(); }

    void CommandStream::Init()
    {
        vk::Device vkDevice = m_device->GetDevice();

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex          = m_device->GetQueueFamilyIndex(m_QueueType);
        poolInfo.flags                     = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_CommandPool = vkDevice.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool                   = m_CommandPool;
        allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount            = 1;

        m_CommandBuffer = vkDevice.allocateCommandBuffers(allocInfo)[0];
    }

    void CommandStream::Destroy()
    {
        vk::Device vkDevice = m_device->GetDevice();

        vkDevice.freeCommandBuffers(m_CommandPool, m_CommandBuffer);
        vkDevice.destroyCommandPool(m_CommandPool);
    }

    void CommandStream::Reset()
    {
        vk::Device vkDevice = m_device->GetDevice();

        vkDevice.resetCommandPool(m_CommandPool, vk::CommandPoolResetFlagBits::eReleaseResources);
    }

    void CommandStream::BeginRecording()
    {
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        m_CommandBuffer.begin(beginInfo);
    }

    void CommandStream::EndRecording() { m_CommandBuffer.end(); }

    void CommandStream::BeginRendering(const vk::RenderingInfo& info) { m_CommandBuffer.beginRendering(info); }

    void CommandStream::EndRendering() { m_CommandBuffer.endRendering(); }

    void CommandStream::BindPipeline(const Pipeline& pipeline)
    {
        vk::PipelineBindPoint bindPoint = m_QueueType == RenderCommandQueueType::Graphics ?
                                              vk::PipelineBindPoint::eGraphics :
                                              vk::PipelineBindPoint::eCompute;
        m_CommandBuffer.bindPipeline(bindPoint, pipeline.GetNativePipeline());
    }

    void CommandStream::DrawIndexed(const DrawIndexCommand& command)
    {
        m_CommandBuffer.drawIndexed(
            command.indexCount, command.instanceCount, command.firstIndex, command.vertexOffset, command.firstInstance);
    }

    void CommandStream::RegisterSignalDependency(vk::Semaphore semaphore) { m_SignalSemQueue.push_back({semaphore}); }

    void CommandStream::RegisterWaitDependency(vk::Semaphore semaphore) { m_WaitSemQueue.push_back(semaphore); }

    void CommandStream::RegisterWaitDependency(vk::PipelineStageFlags waitStage)
    {
        m_WaitStagesQueue.push_back(waitStage);
    }

    void CommandStream::RegisterSignalFence(vk::Fence fence)
    {
        m_Fence     = fence;
        m_NeedFence = true;
    }

    void CommandStream::Flush()
    {
        auto vkDevice = m_device->GetDevice();

        m_SubmitInfo.setCommandBufferCount(1)
            .setCommandBuffers(m_CommandBuffer)
            .setWaitSemaphoreCount(m_WaitSemQueue.size())
            .setWaitSemaphores(m_WaitSemQueue)
            .setWaitDstStageMask(m_WaitStagesQueue)
            .setSignalSemaphoreCount(m_SignalSemQueue.size())
            .setSignalSemaphores(m_SignalSemQueue);

        vk::Queue queue = m_device->GetQueue(m_QueueType);

        if (m_NeedFence)
        {
            queue.submit(m_SubmitInfo, m_Fence);
        }
        else
        {
            queue.submit(m_SubmitInfo, nullptr);
        }

        m_NeedFence = false;
        m_WaitSemQueue.clear();
        m_SignalSemQueue.clear();
        m_WaitStagesQueue.clear();
    }

    void CommandStream::Draw(const DrawCommand& command)
    {
        Buffer* vertexBuffer = m_device->GetResource(command.vertexBuffer);
        Buffer* indexBuffer  = m_device->GetResource(command.indexBuffer);

        if (vertexBuffer == nullptr || indexBuffer == nullptr)
        {
            return;
        }

        uint32_t indexCount = indexBuffer->GetElementCount<uint32_t>();

        m_CommandBuffer.bindVertexBuffers(0, {vertexBuffer->GetBuffer().buffer}, {0});
        m_CommandBuffer.bindIndexBuffer(indexBuffer->GetBuffer().buffer, 0, vk::IndexType::eUint32);
        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, command.pipeline);
        m_CommandBuffer.drawIndexed(
            indexCount, command.instanceCount, command.indexOffset, command.vertexOffset, command.instanceOffset);
    }

    void CommandStream::SetViewport(const vk::Viewport& viewport) { m_CommandBuffer.setViewport(0, viewport); }

    void CommandStream::SetScissor(const vk::Rect2D& scissor) { m_CommandBuffer.setScissor(0, scissor); }

    void CommandStream::ClearColor(const ColorClearCommand& command) {}

    void CommandStream::ClearDepthStencil(const DepthStencilClearCommand& command) {}

    void CommandStream::TransitionImageLayout(const ImageLayoutTransitionCommand& command)
    {
        if (command.oldLayout == command.newLayout)
        {
            return;
        }

        vk::PipelineStageFlags srcStageMask {vk::PipelineStageFlagBits::eTopOfPipe};
        vk::PipelineStageFlags dstStageMask {vk::PipelineStageFlagBits::eTopOfPipe};
        vk::DependencyFlags    dependencyFlags {};
        vk::AccessFlags        srcMask {};
        vk::AccessFlags        dstMask {};

        vk::ImageMemoryBarrier barrier = {};

        vk::ImageLayout oldLayout = command.oldLayout;
        vk::ImageLayout newLayout = command.newLayout;

        typedef vk::ImageLayout    il;
        typedef vk::AccessFlagBits afb;

        // Is it me, or are these the same?
        switch (oldLayout)
        {
            case il::eUndefined:
                break;
            case il::eGeneral:
                srcMask      = afb::eTransferWrite;
                srcStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::eColorAttachmentOptimal:
                srcMask      = afb::eColorAttachmentWrite;
                srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                break;
            case il::eDepthStencilAttachmentOptimal:
                srcMask      = afb::eDepthStencilAttachmentWrite;
                srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;
            case il::eDepthStencilReadOnlyOptimal:
                srcMask      = afb::eDepthStencilAttachmentRead;
                srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;
            case il::eShaderReadOnlyOptimal:
                srcMask      = afb::eShaderRead;
                srcStageMask = vk::PipelineStageFlagBits::eVertexShader;
                break;
            case il::eTransferSrcOptimal:
                srcMask      = afb::eTransferRead;
                srcStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::eTransferDstOptimal:
                srcMask      = afb::eTransferWrite;
                srcStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::ePreinitialized:
                srcMask      = afb::eTransferWrite | afb::eHostWrite;
                srcStageMask = vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eHost;
                break;
            case il::ePresentSrcKHR:
                srcMask = afb::eMemoryRead;
                break;
        }

        switch (newLayout)
        {
            case il::eUndefined:
                break;
            case il::eGeneral:
                dstMask      = afb::eTransferWrite;
                dstStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::eColorAttachmentOptimal:
                dstMask      = afb::eColorAttachmentWrite;
                dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                break;
            case il::eDepthStencilAttachmentOptimal:
                dstMask      = afb::eDepthStencilAttachmentWrite;
                dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;
            case il::eDepthStencilReadOnlyOptimal:
                dstMask      = afb::eDepthStencilAttachmentRead;
                dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;
            case il::eShaderReadOnlyOptimal:
                dstMask      = afb::eShaderRead;
                dstStageMask = vk::PipelineStageFlagBits::eVertexShader;
                break;
            case il::eTransferSrcOptimal:
                dstMask      = afb::eTransferRead;
                dstStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::eTransferDstOptimal:
                dstMask      = afb::eTransferWrite;
                dstStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::ePreinitialized:
                dstMask      = afb::eTransferWrite;
                dstStageMask = vk::PipelineStageFlagBits::eTransfer;
                break;
            case il::ePresentSrcKHR:
                dstMask = afb::eMemoryRead;
                break;
        }

        barrier.setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(command.image)
            .setSubresourceRange(command.subresourceRange)
            .setSrcAccessMask(srcMask)
            .setDstAccessMask(dstMask);

        m_CommandBuffer.pipelineBarrier(srcStageMask, dstStageMask, dependencyFlags, nullptr, nullptr, barrier);
    }
} // namespace wind