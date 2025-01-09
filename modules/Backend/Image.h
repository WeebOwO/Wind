#pragma once

#include <vk_mem_alloc.h>

#include "Device.h"
#include "Resource.h"

namespace wind
{
    struct AllocateImage
    {
        vk::Image     image;
        VmaAllocation allocation;
    };

    class Image : public Resource
    {
    public:
        Image(Device* device) : Resource(device, Tag::Image) {}
        ~Image() override;

        auto GetAllcateImage() { return m_AllocateImage; }

    protected:
        AllocateImage m_AllocateImage;
    };

    using ImageHandle = ResourceHandle<Image>;
} // namespace wind