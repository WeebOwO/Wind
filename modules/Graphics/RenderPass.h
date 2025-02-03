#pragma once

#include "Backend/Resource.h"
#include "Backend/Stream.h"

namespace wind
{
    // todo: mybe we can use this interface to create a render pass
    struct IRenderPass
    {
        virtual void Draw(vk::CommandBuffer cmdBuffer) = 0;
        virtual ~IRenderPass()                         = default;
    };
} // namespace wind