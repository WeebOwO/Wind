#include "Backend/Swapchain.h"

#include "Backend/Device.h"

namespace wind
{
    vk::PresentModeKHR ConvertPresentMode(PresentMode mode)
    {
        switch (mode)
        {
            case PresentMode::Immediate:
                return vk::PresentModeKHR::eImmediate;
            case PresentMode::Mailbox:
                return vk::PresentModeKHR::eMailbox;
            case PresentMode::Fifo:
                return vk::PresentModeKHR::eFifo;
            case PresentMode::FifoRelaxed:
                return vk::PresentModeKHR::eFifoRelaxed;
            default:
                return vk::PresentModeKHR::eFifo;
        }
    }

    Swapchain::Swapchain(Device* device, const SwapchainCreateInfo& createInfo) : Resource(device, Tag::Swapchain)
    {
        auto physicalDevice = device->GetPhysicalDevice();
        auto vkDevice       = device->GetDevice();

        // get surface formats
        auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(device->GetSurface());

        for (auto format : surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                m_format = format.format;
                break;
            }
        }

        vk::SwapchainCreateInfoKHR createInfoNative = {
            .surface          = device->GetSurface(),
            .minImageCount    = createInfo.imageCount,
            .imageFormat      = m_format,
            .imageColorSpace  = vk::ColorSpaceKHR::eSrgbNonlinear,
            .imageExtent      = vk::Extent2D {createInfo.width, createInfo.height},
            .imageArrayLayers = 1,
            .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive,
            .preTransform     = vk::SurfaceTransformFlagBitsKHR::eIdentity,
            .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode      = ConvertPresentMode(createInfo.presentMode),
            .clipped          = VK_TRUE,
            .oldSwapchain     = nullptr,
        };

        m_swapchain = vkDevice.createSwapchainKHR(createInfoNative);

        // extract swapchain images and create image views
        m_images = vkDevice.getSwapchainImagesKHR(m_swapchain);
        m_imageViews.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); i++)
        {
            vk::ImageViewCreateInfo imageViewCreateInfo = {
                .image            = m_images[i],
                .viewType         = vk::ImageViewType::e2D,
                .format           = m_format,
                .components       = {vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity},
                .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
            };

            m_imageViews[i] = vkDevice.createImageView(imageViewCreateInfo);
        }
    }

    Swapchain::~Swapchain()
    {
        auto vkDevice = m_device->GetDevice();
        // destroy image views
        // if we don't using auto& here, it will cause a validation layer error! wtfff
        for (auto& imageView : m_imageViews)
        {
            vkDevice.destroyImageView(imageView);
        }

        vkDevice.destroySwapchainKHR(m_swapchain);
    }

    void Swapchain::Present() {}
} // namespace wind