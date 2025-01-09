#pragma once

#include <string>

#include "RGAllocator.h"

namespace wind::rg
{
    struct RenderGraphTexture
    {
        RenderGraphTexture() noexcept = default;

        struct Descriptor
        {
            uint32_t              width;
            uint32_t              height;
            vk::Format            format  = vk::Format::eUndefined;
            vk::ImageUsageFlags   usage   = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
            vk::AttachmentLoadOp  loadOp  = vk::AttachmentLoadOp::eClear;
            vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore;
            vk::ImageLayout       initialLayout = vk::ImageLayout::eUndefined;
            vk::ImageLayout       finalLayout   = vk::ImageLayout::eUndefined;
            bool                  depth         = false;

            bool operator==(const Descriptor& rhs) const
            {
                return width == rhs.width && height == rhs.height && format == rhs.format;
            }

            bool operator!=(const Descriptor& rhs) const { return !(*this == rhs); }
        };

        // craete the resource
        void Create(rg::RenderGraphAllocator* allocator, const std::string& name, const Descriptor& descriptor);

        Descriptor  desc;
        std::string name;

        vk::Image     image;
        vk::ImageView view;
    };
} // namespace wind::rg