#pragma once

#include <string>

#include <vk_mem_alloc.h>

#include "Core/RefCount.h"

namespace wind
{
    class Device;

    constexpr int INVALID_HANDLE = -1;

    template<typename T>
    struct Handle
    {
        int      index   = INVALID_HANDLE;
        uint32_t version = 0;

        bool IsNull() const { return index == INVALID_HANDLE; }
        void SetNull() { index = INVALID_HANDLE; }
    };

    class Resource : public RefCounted
    {
    public:
        Resource(const Resource&)            = delete;
        Resource& operator=(const Resource&) = delete;

        void SetDebugName(const std::string& name) { m_DebugName = name; }
        auto GetDebugName() const { return m_DebugName; }

    protected:
        Device*     m_Device    = nullptr;
        std::string m_DebugName = "";

        virtual void InitRHI() {}
        virtual void ReleaseRHI() {}

    protected:
        friend class Device;

        explicit Resource(Device* device) : m_Device(device) {}
        virtual ~Resource() = default;
    };
} // namespace wind