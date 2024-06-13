#include "Image.h"

#include "Context.h"

namespace wind
{
    Image::Image(wind::Context* context, const vk::ImageCreateInfo& imageCreateInfo, const VmaAllocationCreateInfo& allocCreateInfo) :
        Resource(context, ResourceType::Image)
    {
        
    }

    Image::~Image() noexcept
    {
        m_context->GetDevice().destroyImageView(m_defaultView);
        m_context->GetDevice().destroyImage(m_handle);
    }

    ImageDimensions Image::GetDimensions() const noexcept { return m_desc.dimensions; }

    uint32_t Image::GetMipLevels() const noexcept { return m_desc.mipLevels; }
} // namespace wind

namespace wind::utils
{
    bool IsDepthFormat(vk::Format format)
    {
        return format == vk::Format::eD16Unorm ||
               format == vk::Format::eD32Sfloat ||
               format == vk::Format::eD16UnormS8Uint ||
               format == vk::Format::eD24UnormS8Uint ||
               format == vk::Format::eD32SfloatS8Uint;
    }
}