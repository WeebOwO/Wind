#pragma once

#include "Device.h"
#include "Guard.h"
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

    protected:
        AllocateImage m_allocateImage;
    };

    class Image2D : public Image
    {
    public:
        Image2D(Device*             device,
                vk::Format          format,
                vk::Extent2D        extent,
                vk::ImageUsageFlags usage);
                
        ~Image2D() override;

    private:
        
    };

} // namespace wind