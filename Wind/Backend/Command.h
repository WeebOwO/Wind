#pragma once

#include "VulkanHeader.h"

#include "RHIResource.h"

namespace wind
{
    class ComputeShader;
    class RasterShader;
    class GPUTexture;
    class GPUBuffer;

    enum class RenderCommandQueueType : uint8_t
    {
        Copy = 0,
        Graphics,
        Compute,
        AsyncCompute,
        General
    };

    class CommandBufferManager
    {
    public:
        CommandBufferManager(GPUDevice& device, uint32_t numThreads, uint32_t numCommandBufferPerThread);
        ~CommandBufferManager();

        void ResetPool(uint32_t frameIndex);

        vk::CommandBuffer GetCommandBuffer(uint32_t frameIndex, uint32_t threadIndex, bool begin);

    private:
        uint32_t CalcPoolIndex(uint32_t frameIndex, uint32_t threadIndex);

        vk::Device                     m_device;
        std::vector<vk::CommandPool>   m_commandPools;
        std::vector<uint8_t>           m_usedBuffers;
        std::vector<vk::CommandBuffer> m_commandBuffers;
        uint32_t                       m_numPoolsPerFrame;
        uint32_t                       m_numCommandBufferPerThread;
    };

} // namespace wind

namespace wind::command
{
    void RenderUI(vk::CommandBuffer cmdBuffer);
    void TransferLayout(vk::CommandBuffer    cb,
                        vk::Image            image,
                        uint32_t             mipCount,
                        uint32_t             layerCount,
                        vk::ImageLayout      oldLayout,
                        vk::ImageLayout      newLayout,
                        vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor);
} // namespace wind::command