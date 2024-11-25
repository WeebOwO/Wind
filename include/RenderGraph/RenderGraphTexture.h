#pragma once

#include <string>

#include "Backend/Image.h"

namespace wind::rg
{
    struct RenderGraphTexture
    {
        RenderGraphTexture() noexcept = default;

        struct Descriptor
        {
            Descriptor() : width(0), height(0), format(vk::Format::eUndefined) {}
            uint32_t   width;
            uint32_t   height;
            vk::Format format;
        };

        Descriptor    desc;
        std::string   name;
        vk::Image     image;
        vk::ImageView view;
    };
} // namespace wind::rg