#include "Command.h"

#include "Device.h"
#include "Engine/RenderConfig.h"

namespace wind
{
    CommandBufferManager::CommandBufferManager(GPUDevice& device,
                                               uint32_t   numThreads,
                                               uint32_t   numCommandBufferPerPool) :
        m_device(device.vkDevice()),
        m_numPoolsPerFrame(numThreads), m_numCommandBufferPerThread(numCommandBufferPerPool)
    {
        m_device                      = device.vkDevice();
        const uint32_t totalPoolCount = numThreads * RenderConfig::MAX_FRAME_IN_FLIGHT;
        // create init command buffer

        uint32_t totalPools = numThreads * RenderConfig::MAX_FRAME_IN_FLIGHT;
        m_usedBuffers.resize(totalPools, 0);
        m_commandPools.resize(totalPools);

        for (auto& cmdPool : m_commandPools)
        {
            vk::CommandPoolCreateInfo poolCreateInfo {
                .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                .queueFamilyIndex = device.queueIndices().mainQueueIndex.value(),
            };

            cmdPool = m_device.createCommandPool(poolCreateInfo);
        }

        // construct all commandBuffer
        uint32_t totalBuffers = totalPools * numCommandBufferPerPool;
        m_commandBuffers.resize(totalBuffers);

        for (uint32_t i = 0; i < totalBuffers; ++i)
        {
            const uint32_t frameIndex  = i / (m_numPoolsPerFrame * m_numCommandBufferPerThread);
            const uint32_t threadIndex = (i / numCommandBufferPerPool) % numThreads;
            const uint32_t poolIndex   = CalcPoolIndex(frameIndex, threadIndex);

            vk::CommandBufferAllocateInfo allocInfo {.commandPool        = m_commandPools[poolIndex],
                                                     .level              = vk::CommandBufferLevel::ePrimary,
                                                     .commandBufferCount = 1};

            m_commandBuffers[i] = m_device.allocateCommandBuffers(allocInfo).front();
        }
    }

    void CommandBufferManager::ResetPool(uint32_t frameIndex)
    {
        for (uint32_t i = 0; i < m_numPoolsPerFrame; ++i)
        {
            uint32_t poolIndex = CalcPoolIndex(frameIndex, i);
            m_device.resetCommandPool(m_commandPools[poolIndex]);

            m_usedBuffers[poolIndex] = 0;
        }
    }

    vk::CommandBuffer CommandBufferManager::GetCommandBuffer(uint32_t frameIndex, uint32_t threadIndex, bool begin)
    {
        const uint32_t poolIndex           = CalcPoolIndex(frameIndex, threadIndex);
        uint32_t       current_used_buffer = m_usedBuffers[poolIndex];
        assert(current_used_buffer < m_numCommandBufferPerThread);

        if (begin)
        {
            m_usedBuffers[poolIndex] = current_used_buffer + 1;
        }

        vk::CommandBuffer cb = m_commandBuffers[(poolIndex * m_numCommandBufferPerThread) + current_used_buffer];
        if (begin)
        {
            cb.reset();
            cb.begin(vk::CommandBufferBeginInfo {});
        }

        return cb;
    }

    CommandBufferManager::~CommandBufferManager()
    {
        m_device.waitIdle();
        for (auto& cmdPool : m_commandPools)
        {
            m_device.destroyCommandPool(cmdPool);
        }
    }

    uint32_t CommandBufferManager::CalcPoolIndex(uint32_t frameIndex, uint32_t threadIndex)
    {
        return (frameIndex * m_numPoolsPerFrame) + threadIndex;
    }

} // namespace wind

namespace wind::command
{
    void RenderUI(vk::CommandBuffer cmdBuffer)
    {
        ImGui::Render();
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmdBuffer);
    }

    void TransferLayout(vk::CommandBuffer    cb,
                        vk::Image            image,
                        uint32_t             mipCount,
                        uint32_t             layerCount,
                        vk::ImageLayout      oldLayout,
                        vk::ImageLayout      newLayout,
                        vk::ImageAspectFlags aspectMask)
    {
        vk::ImageMemoryBarrier imageMemoryBarriers = {};
        imageMemoryBarriers.srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarriers.dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarriers.oldLayout              = oldLayout;
        imageMemoryBarriers.newLayout              = newLayout;
        imageMemoryBarriers.image                  = image;

        imageMemoryBarriers.subresourceRange = {aspectMask, 0, mipCount, 0, layerCount};

        // Put barrier on top
        // https://www.khronos.org/registry/vulkan/specs/1.2/html/chap7.html#synchronization-access-types-supported
        vk::PipelineStageFlags srcStageMask {vk::PipelineStageFlagBits::eTopOfPipe};
        vk::PipelineStageFlags dstStageMask {vk::PipelineStageFlagBits::eTopOfPipe};
        vk::DependencyFlags    dependencyFlags {};
        vk::AccessFlags        srcMask {};
        vk::AccessFlags        dstMask {};

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

        imageMemoryBarriers.srcAccessMask = srcMask;
        imageMemoryBarriers.dstAccessMask = dstMask;
        auto memoryBarriers               = nullptr;
        auto bufferMemoryBarriers         = nullptr;

        cb.pipelineBarrier(
            srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, bufferMemoryBarriers, imageMemoryBarriers);
    }
} // namespace wind::command
