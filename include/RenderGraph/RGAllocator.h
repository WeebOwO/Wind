#pragma once

#include "Backend/Device.h"

namespace wind::rg
{
    class RenderGraphAllocator
    {
    public:
        RenderGraphAllocator(Device* device);
        ~RenderGraphAllocator();

    private:
        Device* m_device;
    };
} // namespace wind::rg