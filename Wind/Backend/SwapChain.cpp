#include "SwapChain.h"

#include "Core/Log.h"

#include "Engine/Window.h"

namespace wind
{
    Swapchain::Swapchain(const GPUDevice& device, const Window& window) : m_device(device)
    {
        VkSurfaceKHR rawSurface;
        glfwCreateWindowSurface(
            static_cast<VkInstance>(m_device.vkInstance()), window.GetWindow(), nullptr, &rawSurface);
        m_surface = rawSurface;
        CreateSwapChainInteral(window.width(), window.height());
        WIND_CORE_INFO("Create swapchain");
        m_vsync = window.IsVsyncEnable();
    }

    void Swapchain::QuerySurfaceProperty()
    {
        auto physicalDevice = m_device.physicalDevice();

        auto presentModes        = physicalDevice.getSurfacePresentModesKHR(m_surface);
        auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        auto surfaceFormats      = physicalDevice.getSurfaceFormatsKHR(m_surface);

        m_surfacePresentMode = m_vsync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eMailbox;

        m_surfaceFormat = surfaceFormats.front();

        for (const auto& availableFormat : surfaceFormats)
        {
            if (availableFormat.format == vk::Format::eR8G8B8A8Unorm &&
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                m_surfaceFormat = availableFormat;
                break;
            }
        }
    }

    Swapchain::~Swapchain()
    {
        auto vkInstance = m_device.vkInstance();
        auto vkDevice   = m_device.vkDevice();
        CleanUpSwapChain();

        vkInstance.destroySurfaceKHR(m_surface);
    }

    void Swapchain::CreateSwapChainInteral(uint32_t width, uint32_t height)
    {
        auto physicalDevice = m_device.physicalDevice();
        auto vkDevice       = m_device.vkDevice();

        auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);

        m_windowExtent = vk::Extent2D(
            std::clamp(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
            std::clamp(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height));

        QuerySurfaceProperty();
        vk::SwapchainCreateInfoKHR swapchainCreateInfo {.surface          = m_surface,
                                                        .minImageCount    = 3,
                                                        .imageFormat      = m_surfaceFormat.format,
                                                        .imageColorSpace  = m_surfaceFormat.colorSpace,
                                                        .imageExtent      = m_windowExtent,
                                                        .imageArrayLayers = 1,
                                                        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment |
                                                                      vk::ImageUsageFlagBits::eTransferDst,
                                                        .imageSharingMode = vk::SharingMode::eExclusive,
                                                        .preTransform     = surfaceCapabilities.currentTransform,
                                                        .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                        .presentMode      = m_surfacePresentMode,
                                                        .clipped          = true,
                                                        .oldSwapchain     = nullptr};

        m_swapchain = vkDevice.createSwapchainKHR(swapchainCreateInfo);

        m_swapchainImages = vkDevice.getSwapchainImagesKHR(m_swapchain);
        WIND_CORE_INFO("SwapChain image count is {}", m_swapchainImages.size());
        m_swapchainViews.resize(m_swapchainImages.size());

        for (uint32_t i = 0; i < m_swapchainImages.size(); ++i)
        {
            vk::ImageSubresourceRange range {.aspectMask     = vk::ImageAspectFlagBits::eColor,
                                             .baseMipLevel   = 0,
                                             .levelCount     = 1,
                                             .baseArrayLayer = 0,
                                             .layerCount     = 1};

            vk::ImageViewCreateInfo viewCreateInfo {.image            = m_swapchainImages[i],
                                                    .viewType         = vk::ImageViewType::e2D,
                                                    .format           = m_surfaceFormat.format,
                                                    .subresourceRange = range};

            m_swapchainViews[i] = vkDevice.createImageView(viewCreateInfo);
        }

        CreateRenderPass();
        ResetClearValue();
    }

    void Swapchain::CreateRenderPass()
    {
        auto vkDevice = m_device.vkDevice();

        vk::AttachmentDescription colorAttachment {
            .format         = m_surfaceFormat.format,
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        };

        vk::AttachmentReference colorReference {.attachment = 0, .layout = vk::ImageLayout::eAttachmentOptimal};

        vk::SubpassDescription subpassDesc {
            .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount = 1,
            .pColorAttachments    = &colorReference,
        };

        vk::SubpassDependency denpendency {.srcSubpass    = VK_SUBPASS_EXTERNAL,
                                           .dstSubpass    = 0,
                                           .srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                           .dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                           .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
                                           .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite};

        vk::RenderPassCreateInfo renderPassCreateInfo {.attachmentCount = 1,
                                                       .pAttachments    = &colorAttachment,
                                                       .subpassCount    = 1,
                                                       .pSubpasses      = &subpassDesc,
                                                       .dependencyCount = 1,
                                                       .pDependencies   = &denpendency};

        m_renderingInfos.resize(m_swapchainViews.size());
        m_attachmentInfos.resize(m_swapchainViews.size());
        // create dynamic rendering part
        for (uint32_t i = 0; i < m_swapchainViews.size(); ++i)
        {
            // setup clear color
            m_attachmentInfos[i] = vk::RenderingAttachmentInfo {
                .sType       = vk::StructureType::eRenderingAttachmentInfo,
                .imageView   = m_swapchainViews[i],
                .imageLayout = vk::ImageLayout::eAttachmentOptimal,
                .loadOp      = vk::AttachmentLoadOp::eClear,
                .storeOp     = vk::AttachmentStoreOp::eStore,
                .clearValue  = m_clearValue,
            };

            m_renderingInfos[i] = vk::RenderingInfo {
                .sType      = vk::StructureType::eRenderingInfo,
                .renderArea = vk::Rect2D {.offset = {}, .extent = {m_windowExtent.width, m_windowExtent.height}},
                .layerCount = 1,
                .colorAttachmentCount = 1,
                .pColorAttachments    = &m_attachmentInfos[i],
            };
        }
    }

    std::optional<uint32_t> Swapchain::AcquireNextImage(const vk::Fence&     waitFence,
                                                        const vk::Semaphore& imageAvailableSemaphore) const
    {
        auto vkDevice = m_device.vkDevice();
        auto _        = vkDevice.waitForFences(waitFence, true, std::numeric_limits<uint64_t>::max());

        if (_ != vk::Result::eSuccess)
        {
            WIND_CORE_ERROR("Fail to get next image index");
        }

        vkDevice.resetFences(waitFence);

        auto result =
            vkDevice.acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore);

        if (result.result != vk::Result::eSuccess)
        {
            return std::nullopt;
        }
        return result.value;
    }

    void Swapchain::SubmitCommandBuffer(const vk::CommandBuffer& cmdBuffer,
                                        const vk::Fence&         signalFence,
                                        const vk::Semaphore&     imageAvailableSemaphore,
                                        const vk::Semaphore&     imageFinishSemaphre,
                                        uint32_t                 imageIndex) const
    {
        std::vector<vk::PipelineStageFlags> waitStage {vk::PipelineStageFlagBits::eColorAttachmentOutput};

        vk::SubmitInfo submitInfo {.waitSemaphoreCount   = 1,
                                   .pWaitSemaphores      = &imageAvailableSemaphore,
                                   .pWaitDstStageMask    = waitStage.data(),
                                   .commandBufferCount   = 1,
                                   .pCommandBuffers      = &cmdBuffer,
                                   .signalSemaphoreCount = 1,
                                   .pSignalSemaphores    = &imageFinishSemaphre};

        m_device.mainQueue().submit(submitInfo, signalFence); // render finish

        vk::PresentInfoKHR presentInfo {.waitSemaphoreCount = 1,
                                        .pWaitSemaphores    = &imageFinishSemaphre,
                                        .swapchainCount     = 1,
                                        .pSwapchains        = &m_swapchain,
                                        .pImageIndices      = &imageIndex};

        auto presentResult = m_device.mainQueue().presentKHR(presentInfo);
    }

    void Swapchain::CleanUpSwapChain()
    {
        auto vkdevice = m_device.vkDevice();

        for (auto& view : m_swapchainViews)
        {
            vkdevice.destroyImageView(view);
        }

        vkdevice.destroySwapchainKHR(m_swapchain);
    }

    void Swapchain::SetClearColor(float r, float g, float b, float a)
    {
        m_swapchainClearColor.setFloat32(std::array<float, 4> {r, g, b, a});
        ResetClearValue();
    }

    void Swapchain::ResetClearValue() { m_clearValue.setColor(m_swapchainClearColor); }
} // namespace wind