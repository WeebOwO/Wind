#pragma once

#include <string>

#include "Backend/Image.h"

namespace wind::rg
{
    class RenderGraphTexture
    {
    public:
        struct Descriptor
        {
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