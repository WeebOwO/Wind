#pragma once

#include <cstdint>

namespace wind
{
    constexpr int INVALID_HANDLE = -1;

    template<typename T>
    struct Handle
    {
        int      index   = INVALID_HANDLE;
        uint32_t version = 0;

        bool IsNull() const { return index == INVALID_HANDLE; }
    };
} // namespace wind