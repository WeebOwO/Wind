#include "Image.h"

#include <vk_mem_alloc.h>

#include "Backend/Device.h"

namespace wind
{
    GPUTexture::GPUTexture(Device* device, const ImageCreateInfo& imageInfo) : Resource(device), imageInfo(imageInfo) {}

    void GPUTexture::InitRHI()
    {
        // Initialize the image and view here
        // ...
        VmaAllocator& vmaAllocator = m_Device->GetAllocator();

        // vma allocation create info
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.preferredFlags          = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        allocInfo.flags                   = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
        allocInfo.usage                   = VMA_MEMORY_USAGE_AUTO;

        // image create info
        vk::ImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.imageType           = vk::ImageType::e2D;
        imageCreateInfo.extent              = imageInfo.extent;
        imageCreateInfo.format              = imageInfo.format;
        imageCreateInfo.usage               = imageInfo.usage;
        imageCreateInfo.mipLevels           = 1;
        imageCreateInfo.arrayLayers         = 1;
        imageCreateInfo.samples             = vk::SampleCountFlagBits::e1;
        imageCreateInfo.tiling              = vk::ImageTiling::eOptimal;
        imageCreateInfo.initialLayout       = vk::ImageLayout::eUndefined;
        imageCreateInfo.sharingMode         = vk::SharingMode::eExclusive;

        // create the image
        vmaCreateImage(vmaAllocator,
                       reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
                       &allocInfo,
                       reinterpret_cast<VkImage*>(&allocateImage.image),
                       &allocateImage.allocation,
                       &allocateImage.allocationInfo);

        // create the image view
        vk::ImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.image                   = allocateImage.image;
        viewCreateInfo.viewType                = vk::ImageViewType::e2D;
        viewCreateInfo.format                  = imageInfo.format;
        viewCreateInfo.components              = {vk::ComponentSwizzle::eIdentity,
                                                  vk::ComponentSwizzle::eIdentity,
                                                  vk::ComponentSwizzle::eIdentity,
                                                  vk::ComponentSwizzle::eIdentity};

        if (!imageInfo.createDefaultView)
        {
            return;
        }
        // todo: may be this not enough to cover all cases
        vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eNone;

        if (imageInfo.usage & vk::ImageUsageFlagBits::eColorAttachment)
        {
            aspectMask = vk::ImageAspectFlagBits::eColor;
        }
        else if (imageInfo.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        {
            aspectMask = vk::ImageAspectFlagBits::eDepth;
        }

        viewCreateInfo.subresourceRange.aspectMask     = aspectMask;
        viewCreateInfo.subresourceRange.baseMipLevel   = 0;
        viewCreateInfo.subresourceRange.levelCount     = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount     = 1;

        // create the image view
        imageView = m_Device->GetDevice().createImageView(viewCreateInfo);
    }

    void GPUTexture::ReleaseRHI()
    {
        // Release the image and view here
        // ...
        if (allocateImage.allocation)
        {
            if (imageView)
            {
                m_Device->GetDevice().destroyImageView(imageView);
                imageView = nullptr;
            }
            vmaDestroyImage(m_Device->GetAllocator(), allocateImage.image, allocateImage.allocation);
        }
    }
} // namespace wind