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

        vk::Format       GetFormat() const { return m_format; }
        vk::SwapchainKHR GetSwapchain() const { return m_swapchain; }
        vk::Image        GetImage(uint32_t index) const { return m_images[index]; }
        vk::ImageView    GetImageView(uint32_t index) const { return m_imageViews[index]; }

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

    private:
        uint32_t         m_width;
        uint32_t         m_height;
        vk::Format       m_format;
        vk::SwapchainKHR m_swapchain;

        // swapchain images
        std::vector<vk::Image>     m_images;
        std::vector<vk::ImageView> m_imageViews;
    };
} // namespace wind