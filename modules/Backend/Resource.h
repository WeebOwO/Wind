#pragma once

#include <string>

#include <vk_mem_alloc.h>

#include "Core/RefCount.h"

namespace wind
{
    class Device;
    
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