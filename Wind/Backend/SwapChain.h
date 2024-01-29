#pragma once

#include "Device.h"
#include "Texture.h"
#include "VulkanHeader.h"

namespace wind
{

    class Window;
    class Swapchain
    {
    public:
        Swapchain(const GPUDevice& device, const Window& window);
        ~Swapchain();

        void     Resize(uint32_t width, uint32_t height);
        uint32_t ImageCount() const { return m_swapchainImages.size(); }

        auto imageView(uint32_t index) const { return m_swapchainViews[index]; }
        auto image(uint32_t index) const { return m_swapchainImages[index]; }
        auto format() const { return m_surfaceFormat.format; }
        auto clearValue() const { return &m_clearValue; }
        auto renderingInfo(uint32_t index) const { return m_renderingInfos[index]; }

        std::pair<uint32_t, uint32_t> extent() const { return {m_windowExtent.width, m_windowExtent.height}; }
        uint32_t                      width() const { return m_windowExtent.width; }
        uint32_t                      height() const { return m_windowExtent.height; }

        float aspectRatio() const
        {
            return static_cast<float>(m_windowExtent.width) / static_cast<float>(m_windowExtent.height);
        }

        void SetFrameNumber(uint32_t currentFrame) { m_frameNumber = currentFrame; }
        void SetClearColor(float r, float g, float b, float a);

        std::optional<uint32_t> AcquireNextImage(const vk::Fence&     waitFence,
                                                 const vk::Semaphore& imageAvailableSemaphore) const;

        void SubmitCommandBuffer(const vk::CommandBuffer& cmdBuffer,
                                 const vk::Fence&         signalFence,
                                 const vk::Semaphore&     imageAvailableSemaphore,
                                 const vk::Semaphore&     imageFinishSemaphre,
                                 uint32_t                 imageIndex) const;

    private:
        void QuerySurfaceProperty();
        void GetSwapChainImage();
        void CreateSwapChainInteral(uint32_t width, uint32_t height);
        void CleanUpSwapChain();
        void CreateRenderPass();

        void             ResetClearValue();
        const GPUDevice& m_device;

        std::vector<vk::Image>     m_swapchainImages;
        std::vector<vk::ImageView> m_swapchainViews;

        vk::SwapchainKHR m_swapchain;
        vk::SurfaceKHR   m_surface;

        vk::Extent2D m_windowExtent;

        vk::SurfaceFormatKHR m_surfaceFormat;
        vk::PresentModeKHR   m_surfacePresentMode;

        vk::ClearColorValue m_swapchainClearColor {std::array<float, 4> {0.3f, 0.3f, 0.3f, 0.3f}};
        vk::ClearValue      m_clearValue {};
        bool                m_vsync {true};

        // test dynamic rendering
        std::vector<vk::RenderingAttachmentInfo> m_attachmentInfos;
        std::vector<vk::RenderingInfo>           m_renderingInfos;
        uint32_t                                 m_frameNumber;
    };

}; // namespace wind