#pragma once

#include <cstdint>

#include "Resource.h"

namespace wind
{
    enum class MemoryType : uint8_t
    {
        Device,
        Upload,
        ReadBack,
    };

    class Buffer : public Resource
    {
    public:
        struct Desc
        {
            uint64_t   size;
            MemoryType memoryType;
        };

        Buffer(Context* context, const Desc& desc);

    private:
        Desc m_desc;
    };
} // namespace wind