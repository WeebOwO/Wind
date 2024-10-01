#pragma once

#include "Enum.h"
#include "Guard.h"
#include "Resource.h"

namespace wind
{
    struct SwapchainCreateInfo
    {
        uint32_t    width;
        uint32_t    height;
        uint32_t    imageCount;
        PresentMode presentMode;
    };

    class Swapchain : public Resource
    {
    public:
        Swapchain(Device* device, const SwapchainCreateInfo& createInfo);
        ~Swapchain() override;

        void Present();

        void BeginFrame();
        void EndFrame();

    private:
        vk::Format       m_format;
        vk::SwapchainKHR m_swapchain;

        // swapchain images
        std::vector<vk::Image>     m_images;
        std::vector<vk::ImageView> m_imageViews;
    };
} // namespace wind