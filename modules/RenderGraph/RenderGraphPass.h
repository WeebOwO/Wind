#pragma once

#include "Backend/Resource.h"
#include "Backend/Stream.h"

namespace wind
{
    class RenderGraph;

    class RenderGraphPass
    {
    public:
        virtual void Draw(vk::CommandBuffer cmdBuffer) = 0;
        virtual ~RenderGraphPass()                     = default;
    };
} // namespace wind