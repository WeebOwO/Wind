#pragma once

namespace wind
{
    class Context;
    enum ResourceType
    {
        Buffer = 0,
        Image,
        SwapChain,
        CommandBuffer,
        Count
    };

    class Resource
    {
    public:
        friend class Context;
        explicit Resource(Context* context, ResourceType type) : m_type(type), m_context(context) {}
        virtual ~Resource() = default;

    protected:
        ResourceType m_type;
        Context*     m_context;
    };
} // namespace wind