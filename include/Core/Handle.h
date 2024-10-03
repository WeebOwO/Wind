#pragma once

#include <cstdint>

namespace wind
{
    constexpr uint32_t INVALID_HANDLE = -1;

    template<typename T>
    class Handle
    {
    public:
        using ValueType = T;

        Handle() = default;
        Handle(uint32_t index) : m_index(index) {}

        T*   Get() { return m_object; }
        bool IsValid() { return m_index != INVALID_HANDLE; }

        virtual void Init() {}
        virtual void Destroy() {}

    protected:
        uint32_t m_index  = INVALID_HANDLE;
        T*       m_object = nullptr;
    };

} // namespace wind