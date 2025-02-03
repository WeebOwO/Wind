#include "Swapchain.h"

#include "Device.h"

namespace wind
{
    Swapchain::Swapchain(Device* device, uint32_t width, uint32_t height) : Resource(device)
    {
        m_Width  = width;
        m_Height = height;

        auto physicalDevice = device->GetPhysicalDevice();
        auto vkDevice       = device->GetDevice();

        // get surface formats
        auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(device->GetSurface());

        for (auto format : surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                m_Format = format.format;
                break;
            }
        }

        vk::SwapchainCreateInfoKHR createInfoNative = 
        {
            .surface          = device->GetSurface(),
            .minImageCount    = 3,
            .imageFormat      = m_Format,
            .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
            .imageExtent      = vk::Extent2D {m_Width, m_Height},
            .imageArrayLayers = 1,
            .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive,
            .preTransform     = vk::SurfaceTransformFlagBitsKHR::eIdentity,
            .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode      = vk::PresentModeKHR::eFifo,
            .clipped          = VK_TRUE,
            .oldSwapchain     = nullptr,
        };

        m_Swapchain = vkDevice.createSwapchainKHR(createInfoNative);

        // extract swapchain images and create image views
        m_Images = vkDevice.getSwapchainImagesKHR(m_Swapchain);
        m_ImageViews.resize(m_Images.size());

        for (size_t i = 0; i < m_Images.size(); i++)
        {
            vk::ImageViewCreateInfo imageViewCreateInfo = {
                .image            = m_Images[i],
                .viewType         = vk::ImageViewType::e2D,
                .format           = m_Format,
                .components       = {vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity},
                .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
            };

            m_ImageViews[i] = vkDevice.createImageView(imageViewCreateInfo);
        }
    }

    Swapchain::~Swapchain()
    {
        auto vkDevice = m_Device->GetDevice();

        for (auto imageView : m_ImageViews)
        {
            vkDevice.destroyImageView(imageView);
        }

        vkDevice.destroySwapchainKHR(m_Swapchain);
    }
} // namespace wind