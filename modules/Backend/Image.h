#pragma once

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    struct AllocateImage
    {
        vk::Image         image;
        VmaAllocation     allocation;
        VmaAllocationInfo allocationInfo;
    };

    struct DataSpan
    {
        void*  data;
        size_t size;
    };

    struct ImageCreateInfo
    {
        std::string         debugName;
        vk::Extent3D        extent;
        vk::Format          format;
        vk::ImageUsageFlags usage;
        DataSpan            data;
        bool                mipMap {false};
        bool                createDefaultView {true};
    };

    struct GPUTexture : public Resource
    {
        GPUTexture(Device* device, const ImageCreateInfo& imageInfo);

        void InitRHI() override;
        void ReleaseRHI() override;

        ImageCreateInfo imageInfo;
        AllocateImage   allocateImage;

        vk::ImageView imageView {nullptr};
    };
} // namespace wind