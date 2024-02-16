#include "RenderGraphTexture.h"
#include "Backend/Device.h"

namespace wind
{
    static vk::ImageType GetImageType(TextureViewType viewType)
    {
        if (viewType == TextureViewType::Texture2D || viewType == TextureViewType::Texture2DArray ||
            viewType == TextureViewType::CubeMap)
        {
            return vk::ImageType::e2D;
        }
        if (viewType == TextureViewType::Texture3D)
        {
            return vk::ImageType::e3D;
        }
        return vk::ImageType::e1D;
    };

    static vk::ImageViewType GetImageViewType(TextureViewType viewtype)
    {
        switch (viewtype)
        {
            case TextureViewType::Texture1D:
                return vk::ImageViewType::e1D;
            case TextureViewType::Texture2D:
                return vk::ImageViewType::e2D;
                break;
            case TextureViewType::Texture2DArray:
                return vk::ImageViewType::e2DArray;
                break;
            case TextureViewType::Texture3D:
                return vk::ImageViewType::e3D;
                break;
            case TextureViewType::CubeMap:
                return vk::ImageViewType::eCube;
            case TextureViewType::CubeMapArray:
                return vk::ImageViewType::eCubeArray;
        }
        return {};
    }

    vk::Image     RenderGraphTexture::GetImage() const noexcept { return m_texture->image(); }
    vk::ImageView RenderGraphTexture::GetImageView() const noexcept { return m_texture->view(); }

    RenderGraphTexture::RenderGraphTexture(const Desc& desc) noexcept :
        RenderGraphResource(RenderGraphResourceType::Texture), m_desc(desc)
    {}

    void RenderGraphTexture::SetTexture(GPUTextureRef texture) { m_texture = texture; }

    void RenderGraphTexture::InitRHI()
    {
        auto& device = GPUDevice::Get();
        // todo: if someone call this twice, we need to check the new desc is same with old ?
        uint32_t mipCount = utils::CalculateImageMipLevelCount(m_desc.width, m_desc.height, m_desc.depth);

        vk::ImageCreateInfo createInfo {
            .imageType     = GetImageType(m_desc.type),
            .format        = m_desc.format,
            .extent        = {.width = m_desc.width, .height = m_desc.height, .depth = m_desc.depth},
            .mipLevels     = mipCount,
            .arrayLayers   = 1,
            .samples       = vk::SampleCountFlagBits::e1,
            .usage         = m_desc.usage,
            .initialLayout = vk::ImageLayout::eUndefined};

        m_texture = device.CreateGPUTexture(createInfo);

        m_texture->CreateDefaultImageView(
            vk::ImageSubresourceRange {.aspectMask     = utils::ImageFormatToImageAspect(m_desc.format),
                                       .baseMipLevel   = 0,
                                       .levelCount     = 1,
                                       .baseArrayLayer = 0,
                                       .layerCount     = 1},
            GetImageViewType(m_desc.type));
    }

    RenderGraphTexture::~RenderGraphTexture() noexcept { ReleaseRHI(); }

    void RenderGraphTexture::ReleaseRHI() { m_texture.reset(); }

} // namespace wind

namespace wind::utils
{
    RenderGraphTexture::Desc GetRenderTargetDesc(uint32_t width, uint32_t height, vk::Format format, bool useMipmap)
    {
        return RenderGraphTexture::Desc {.width     = width,
                                         .height    = height,
                                         .depth     = 1,
                                         .format    = format,
                                         .layout    = vk::ImageLayout::eColorAttachmentOptimal,
                                         .usage     = vk::ImageUsageFlagBits::eColorAttachment,
                                         .useMipmap = useMipmap};
    }
} // namespace wind::utils