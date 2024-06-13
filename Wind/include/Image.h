#pragma once

#include "vk_mem_alloc.h"

#include "Resource.h"
#include "VulkanGuard.h"

namespace wind
{
    enum class ImageType
    {
        e1D,
        e2D,
        e3D,
        eCube
    };

    class ImageDimensions
    {
    public:
        ImageDimensions() = default;

        static ImageDimensions Create2D(uint32_t width, uint32_t height)
        {
            return ImageDimensions {ImageType::e2D, width, height, 1};
        }

        static ImageDimensions Create3D(uint32_t width, uint32_t height, uint32_t depth)
        {
            return ImageDimensions {ImageType::e3D, width, height, depth};
        }

    private:
        friend class Image;
        ImageDimensions(ImageType type, uint32_t width, uint32_t height, uint32_t depth) :
            m_type(type), m_width(width), m_height(height), m_depth(depth)
        {}
        ImageType m_type;
        uint32_t  m_width;
        uint32_t  m_height;
        uint32_t  m_depth;
    };

    struct ImageDesc
    {
        ImageDimensions dimensions;
        vk::Format      format;
        vk::ImageLayout layout;
        uint32_t        mipLevels;
    };

    class Image : public Resource
    {
    public:
        ~Image() noexcept override;

        [[nodiscard]] uint32_t        GetMipLevels() const noexcept;
        [[nodiscard]] ImageDimensions GetDimensions() const noexcept;

        [[nodiscard]] vk::Image     Handle() const { return m_handle; }
        [[nodiscard]] vk::ImageView DefaultView() const { return m_defaultView; }

        void SetImageLayout(vk::ImageLayout newLayout);
        void GenerateMipmaps();

    private:
        friend class Context;

        Image(Context*                       context,
              const vk::ImageCreateInfo&     imageCreateInfo,
              const VmaAllocationCreateInfo& allocCreateInfo);

        ImageDesc     m_desc;
        vk::Image     m_handle;
        vk::ImageView m_defaultView;
    };
} // namespace wind

namespace wind::utils
{
    bool IsDepthFormat(vk::Format format);
}