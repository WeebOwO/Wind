#include "Command.h"

#include "Backend/RHIResource.h"
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
} // namespace wind