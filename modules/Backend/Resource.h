#pragma once

#include "Enum.h"

namespace wind
{
    constexpr uint32_t kInvalidBackendID = std::numeric_limits<uint32_t>::max();

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
            CommandStream
        };

    protected:
        Tag           m_tag    = Tag::None;
        Device*       m_device = nullptr;
        bool          m_init   = false;
        ResourceState m_state  = ResourceState::UnDefined;

    protected:
        friend class Device;

        explicit Resource(Device* device, Tag tag) : m_device(device), m_tag(tag) { Init(); }

        virtual ~Resource() { Destroy(); }

        void Init()
        {
            // todo: add assert here to make sure this is called on the render thread
            m_init = true;
        }

        void Destroy()
        {
            // todo: add assert here to make sure this is called on the render thread
            m_init = false;
        }
    };

    template<typename T>
    class ResourceHandle
    {
        using type = T;
        int id     = kInvalidBackendID;
    };
} // namespace wind