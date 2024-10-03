#pragma once

#include "Enum.h"

namespace wind
{
    class Device;

    class Resource
    {
    public:
        enum class Tag
        {
            None,
            Shader,
            Buffer,
            Image,
            Swapchain,
            CommandStream
        };

    protected:
        Device*       m_device = nullptr;
        ResourceState m_state  = ResourceState::UnDefined;
        Tag           m_tag    = Tag::None;

    public:
        explicit Resource(Device* device, Tag tag) : m_device(device), m_tag(tag) {}
        virtual ~Resource() = default;

        virtual void Init() {}
        virtual void Destroy() {}
    };
} // namespace wind