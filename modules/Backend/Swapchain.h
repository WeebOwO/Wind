#pragma once

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    class Swapchain : public Resource
    {
    public:
        Swapchain(Device* device, uint32_t width, uint32_t height);
        ~Swapchain() override;

        void Present();

        vk::Format        GetFormat() const { return m_Format; }
        vk::SwapchainKHR  GetSwapchain() const { return m_Swapchain; }
        vk::SwapchainKHR* GetSwapchainPtr() { return &m_Swapchain; }
        vk::Extent2D     GetExtent() const { return {m_Width, m_Height}; }

        vk::Image     GetImage(uint32_t index) const { return m_Images[index]; }
        vk::ImageView GetImageView(uint32_t index) const { return m_ImageViews[index]; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        uint32_t         m_Width;
        uint32_t         m_Height;
        vk::Format       m_Format;
        vk::SwapchainKHR m_Swapchain;

        // swapchain images
        std::vector<vk::Image>     m_Images;
        std::vector<vk::ImageView> m_ImageViews;
    };
} // namespace wind