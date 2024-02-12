#pragma once

#include "VulkanHeader.h"
#include "SwapChain.h"

namespace wind
{
    class Window;
    class GPUDevice;

    // this part use c-interface vulkan
    class WindUIContext
    {
    public:
        WindUIContext(const GPUDevice& device, const Window& window, const Swapchain& swapchain);
        ~WindUIContext();

    private:
        const GPUDevice& m_device;
        VkDescriptorPool m_imguiPool;
        VkRenderPass     m_renderPass;
    };
} // namespace wind