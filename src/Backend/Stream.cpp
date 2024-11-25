#pragma once

#include "Backend/Stream.h"
#include "Backend/Device.h"
#include "vulkan/vulkan_enums.hpp"

namespace wind
{
    CommandStream::CommandStream(Device* device, RenderCommandQueueType queueType) :
        Resource(device, Tag::CommandStream), m_queueType(queueType)
    {
        Init();
    }

    CommandStream::~CommandStream() noexcept { Destroy(); }

    void CommandStream::Init()
    {
        vk::Device vkDevice = m_device->GetDevice();

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex          = m_device->GetQueueFamilyIndex(m_queueType);
        poolInfo.flags                     = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_commandPool = vkDevice.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool                   = m_commandPool;
        allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount            = 1;

        m_commandBuffer = vkDevice.allocateCommandBuffers(allocInfo)[0];
    }

    void CommandStream::Destroy()
    {
        vk::Device vkDevice = m_device->GetDevice();

        vkDevice.freeCommandBuffers(m_commandPool, m_commandBuffer);
        vkDevice.destroyCommandPool(m_commandPool);
    }

    void CommandStream::BeginRendering(const vk::RenderingInfo& info) { m_commandBuffer.beginRendering(info); }

    void CommandStream::EndRendering() { m_commandBuffer.endRendering(); }

    void CommandStream::BindPipeline(const Pipeline& pipeline)
    {
        vk::PipelineBindPoint bindPoint = m_queueType == RenderCommandQueueType::Graphics ?
                                              vk::PipelineBindPoint::eGraphics :
                                              vk::PipelineBindPoint::eCompute;
        m_commandBuffer.bindPipeline(bindPoint, pipeline.GetNativePipeline());
    }
} // namespace wind