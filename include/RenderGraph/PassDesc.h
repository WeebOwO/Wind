#pragma once

#include "Backend/Enum.h"
#include "RenderGraphID.h"
#include "RenderGraphTexture.h"
#include "vulkan/vulkan_structs.hpp"

namespace wind::rg
{
    struct RenderPassDesc
    {
        static constexpr size_t kAttachmentMax = kMaxRenderTargets + 2;
        struct Attachments
        {
            union
            {
                RenderGraphID<RenderGraphTexture> textures[kAttachmentMax];
                struct
                {
                    RenderGraphID<RenderGraphTexture> color[kMaxRenderTargets];
                    RenderGraphID<RenderGraphTexture> depth;
                    RenderGraphID<RenderGraphTexture> stencil;
                };
            };
        };

        struct Descriptor
        {
            Attachments  attachments;
            vk::Viewport viewPort;
        };
    };

    struct ComputePassDesc
    {};
} // namespace wind