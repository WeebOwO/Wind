#pragma once

#include "Backend/Device.h"

namespace wind::rg
{
    class RenderGraphAllocator
    {
    public:
        RenderGraphAllocator(Device* device);
        ~RenderGraphAllocator();

        uint32_t AllocateResource();

    private:
        Device* m_device;
    };
} // namespace wind::rg