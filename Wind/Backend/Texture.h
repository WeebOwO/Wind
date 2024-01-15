#pragma once

#include "Allocator.h"
#include "RHIResource.h"
#include "VulkanHeader.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_vulkan.h"

namespace wind
{
    enum class TextureViewType
    {
        Texture1D,
        Texture2D,
        Texture2DArray,
        Texture3D,
        CubeMap,
        CubeMapArray
    };

    struct GPUTexture : public RHIResource<RHIResourceType::Texture>
    {
    public:
        GPUTexture() = default;
        GPUTexture(const vk::ImageCreateInfo& createInfo);
        ~GPUTexture();

        [[deprecated("Will deprecate in next version")]] static Ref<GPUTexture>
        Create(const vk::ImageCreateInfo& createInfo);

        void CreateDefaultImageView(const vk::ImageSubresourceRange& range, vk::ImageViewType viewType);

        vk::ImageLayout GetImageLayout() const noexcept { return m_layout; }
        vk::ImageView   GetView() const { return m_defaultView; } // need to make sure you call CreateImageView before
        vk::Image       GetVkImage() const noexcept { return m_allocatedImage.image; }
        vk::Sampler     GetDefaultSampler() const noexcept { return m_defaultSampler; }

        vk::ImageSubresourceRange GetDefaultImageSubresourceRange() const;
        vk::ImageSubresourceRange GetImageSubresourceRange(uint32_t mip, uint32_t level = 0) const;

        void MarkUseByImgui(const ImVec2& size,
                            const ImVec2& uv0,
                            const ImVec2& uv1,
                            const ImVec4& tint_col   = ImVec4(1, 1, 1, 1),
                            const ImVec4& border_col = ImVec4(0, 0, 0, 0));

        operator vk::Image() { return m_allocatedImage.image; }

        void SetImageLayout(vk::ImageLayout layout) noexcept;

    private:
        uint32_t                   m_width;
        uint32_t                   m_height;
        uint32_t                   m_depth;
        uint32_t                   m_mipCount;
        uint32_t                   m_layerCount;
        vk::Format                 m_format;
        vk::ImageUsageFlags        m_usage;
        vk::SampleCountFlagBits    m_sampleCount = vk::SampleCountFlagBits::e1;
        vk::ImageLayout            m_layout      = vk::ImageLayout::eUndefined;
        AllocatedImage             m_allocatedImage;
        vk::Sampler                m_defaultSampler;
        vk::ImageView              m_defaultView;
        std::vector<vk::ImageView> m_cubeMapViews; // only useful when we create cubemap
        vk::DescriptorSet          m_imguiSet;     // only useful when need to sample by imgui context
    };
} // namespace wind

namespace wind::utils
{
    // human driver part, we deduce the result from image usage and format
    vk::ImageAspectFlags   ImageFormatToImageAspect(vk::Format format);
    vk::ImageLayout        ImageUsageToImageLayout(vk::ImageUsageFlagBits usage);
    vk::AccessFlags        ImageUsageToAccessFlags(vk::ImageUsageFlagBits usage);
    vk::PipelineStageFlags ImageUsageToPipelineStage(vk::ImageUsageFlagBits usage);

    vk::AccessFlags ImageLayoutToAccessFlags(vk::ImageLayout layout);

    bool     IsDepthFormat(vk::Format format);
    uint32_t CalculateImageMipLevelCount(uint32_t width, uint32_t height, uint32_t depth = 1);
} // namespace wind::utils
