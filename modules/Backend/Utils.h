#pragma once

#include <optional>

#include "Guard.h"

namespace wind::init
{
    inline vk::RenderingAttachmentInfo GetColorAttachmentInfo(vk::ImageView                 imageView,
                                                              vk::Format                    format,
                                                              vk::ImageLayout               layout,
                                                              std::optional<vk::ClearValue> clearValue)
    {
        vk::RenderingAttachmentInfo info;
        info.imageView   = imageView;
        info.imageLayout = layout;
        info.loadOp      = clearValue == std::nullopt ? vk::AttachmentLoadOp::eDontCare : vk::AttachmentLoadOp::eClear;
        info.storeOp     = vk::AttachmentStoreOp::eStore;
        if (clearValue.has_value())
        {
            info.clearValue = clearValue.value();
        }
        return info;
    }

    inline vk::RenderingAttachmentInfo GetDepthAttachmentInfo(vk::ImageView imageView, vk::ImageLayout layout)
    {
        vk::RenderingAttachmentInfo info;
        info.imageView               = imageView;
        info.imageLayout             = layout;
        info.loadOp                  = vk::AttachmentLoadOp::eClear;
        info.storeOp                 = vk::AttachmentStoreOp::eStore;
        info.clearValue.depthStencil = {1.0f, 0};
        return info;
    }

    inline vk::RenderingInfo GetRenderingInfo(vk::Rect2D                   renderArea,
                                              vk::RenderingAttachmentInfo* colorAttachment,
                                              vk::RenderingAttachmentInfo* depthAttachment)
    {
        vk::RenderingInfo info;
        info.renderArea           = renderArea;
        info.colorAttachmentCount = 1;
        info.pColorAttachments    = colorAttachment;
        info.pDepthAttachment     = depthAttachment;
        info.layerCount           = 1;
        info.pStencilAttachment   = nullptr;
        return info;
    }
} // namespace wind::init

namespace wind::utils
{
    inline void TransitionImageLayout(vk::CommandBuffer cmdBuffer,
                                      vk::Image         image,
                                      vk::ImageLayout   oldLayout,
                                      vk::ImageLayout   newLayout,
                                      uint32_t          mipLevels)
    {
        vk::ImageMemoryBarrier2 imageBarrier {};
        imageBarrier.pNext = nullptr;

        imageBarrier.srcStageMask  = vk::PipelineStageFlagBits2::eAllCommands;
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eMemoryWrite;
        imageBarrier.dstStageMask  = vk::PipelineStageFlagBits2::eAllCommands;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead;

        imageBarrier.oldLayout = oldLayout;
        imageBarrier.newLayout = newLayout;

        vk::ImageAspectFlags aspectMask = (newLayout == vk::ImageLayout::eDepthAttachmentOptimal) ?
                                              vk::ImageAspectFlagBits::eDepth :
                                              vk::ImageAspectFlagBits::eColor;

        imageBarrier.subresourceRange = {aspectMask, 0, vk::RemainingMipLevels, 0, 1};
        imageBarrier.image            = image;

        vk::DependencyInfo depInfo {};
        depInfo.pNext = nullptr;

        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers    = &imageBarrier;

        cmdBuffer.pipelineBarrier2(depInfo);
    }
} // namespace wind::utils