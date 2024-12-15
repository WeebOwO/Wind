#pragma once

#include "Backend/Stream.h"
#include "Backend/Device.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"

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

    void CommandStream::BeginRendering(const vk::RenderingInfo& info) { m_CommandBuffer.beginRendering(info); }

    void CommandStream::EndRendering() { m_CommandBuffer.endRendering(); }

    void CommandStream::BindPipeline(const Pipeline& pipeline)
    {
        vk::PipelineBindPoint bindPoint = m_QueueType == RenderCommandQueueType::Graphics ?
                                              vk::PipelineBindPoint::eGraphics :
                                              vk::PipelineBindPoint::eCompute;
        m_CommandBuffer.bindPipeline(bindPoint, pipeline.GetNativePipeline());
    }

    void CommandStream::DrawIndex(const DrawIndexCommand& command) 
    {
        m_CommandBuffer.drawIndexed(command.indexCount, command.instanceCount, command.firstIndex, command.vertexOffset, command.firstInstance);
    }

    void CommandStream::RegisterSignalDependency(vk::Semaphore semaphore) 
    { 
        m_SignalQueue.push_back(semaphore); 
    }

    void CommandStream::RegisterWaitDependency(vk::Semaphore semaphore) 
    { 
        m_WaitQueue.push_back(semaphore); 
    }

    void CommandStream::Flush() 
    {
        auto vkDevice = m_device->GetDevice();

        m_SubmitInfo.setCommandBufferCount(1)
                    .setCommandBuffers(m_CommandBuffer)
                    .setWaitSemaphoreCount(m_WaitQueue.size())
                    .setWaitSemaphores(m_WaitQueue)
                    .setSignalSemaphoreCount(m_SignalQueue.size())
                    .setSignalSemaphores(m_SignalQueue);
        
        vk::Queue queue = m_device->GetQueue(m_QueueType);

        queue.submit(m_SubmitInfo, nullptr);
    }
} // namespace wind