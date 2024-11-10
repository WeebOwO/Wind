#pragma once

#include "Device.h"
#include "Resource.h"
#include <vk_mem_alloc.h>


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

        auto GetAllcateImage() { return m_allocateImage; }

    protected:
        AllocateImage m_allocateImage;
    };

    using ImageHandle = ResourceHandle<Image>;
} // namespace wind