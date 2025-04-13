#pragma once

namespace wind
{
    class Device;

    class RenderGraphAllocator
    {
    public:
        RenderGraphAllocator(Device* device) : m_Device(device) {}
        ~RenderGraphAllocator() = default;

    private:
        Device* m_Device = nullptr;
    };
} // namespace wind