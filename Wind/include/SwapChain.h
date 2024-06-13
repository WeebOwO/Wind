#pragma once

#include "Resource.h"
#include "VulkanGuard.h"

namespace wind
{
    struct SwapChainDesc
    {
        uint32_t width;
        uint32_t height;
        uint32_t imageCount;
        bool     vsync;
    };

    class SwapChain : public Resource
    {
    public:
        ~SwapChain();

        void Resize(uint32_t width, uint32_t height);

    private:
        SwapChain(Context* context, const SwapChainDesc& desc);
        SwapChainDesc m_desc;

        std::vector<vk::Image>     m_swapChainImages;
        std::vector<vk::ImageView> m_swapChainImageViews;
    };
} // namespace wind