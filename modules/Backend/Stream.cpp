#include "Stream.h"

#include "Device.h"

namespace wind
{
    CommandStream::CommandStream(Device* device, CommandQueueType queueType, StreamMode mode) :
        Resource(device), m_QueueType(queueType), m_Mode(mode)
    {}

    void CommandStream::InitRHI()
    {
        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex          = m_Device->GetQueue(m_QueueType).familyIndex;
        poolInfo.flags                     = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_CommandPool = m_Device->GetDevice().createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool                   = m_CommandPool;
        allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount            = 1;

        m_CommandBuffer = m_Device->GetDevice().allocateCommandBuffers(allocInfo)[0];
    }

    void CommandStream::ReleaseRHI()
    {
        m_Device->GetDevice().freeCommandBuffers(m_CommandPool, m_CommandBuffer);
        m_Device->GetDevice().destroyCommandPool(m_CommandPool);
    }

    void CommandStream::Reset() { m_Device->GetDevice().resetCommandPool(m_CommandPool); }

    vk::CommandBuffer CommandStream::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        m_CommandBuffer.begin(beginInfo);
        m_IsRecording = true;

        return m_CommandBuffer;
    }

    void CommandStream::Draw(const DrawCommand& draw)
    {
        GPUBuffer* vertexBuffer = m_Device->Get(draw.vertexBuffer);
        GPUBuffer* indexBuffer  = m_Device->Get(draw.indexBuffer);

        m_CommandBuffer.bindVertexBuffers(0, {vertexBuffer->GetNativeHandle()}, {0});
        m_CommandBuffer.bindIndexBuffer(indexBuffer->GetNativeHandle(), 0, vk::IndexType::eUint32);
        m_CommandBuffer.drawIndexed(draw.indexCount, draw.instanceCount, 0, 0, 0);
    }
} // namespace wind