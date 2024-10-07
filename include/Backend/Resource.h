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
            Pipeline,
            Swapchain,
            RenderPass,
            Material,
            CommandStream
        };

    protected:
        Device*       m_device = nullptr;
        bool          m_init   = false;
        ResourceState m_state  = ResourceState::UnDefined;
        Tag           m_tag    = Tag::None;

    protected:
        explicit Resource(Device* device, Tag tag) : m_device(device), m_tag(tag) { Init(); }

        virtual ~Resource() { Destroy(); }

        virtual void Init()
        {
            // todo: add assert here to make sure this is called on the render thread
            m_init = true;
        }

        virtual void Destroy()
        {
            // todo: add assert here to make sure this is called on the render thread
            m_init = false;
        }
    };
} // namespace wind