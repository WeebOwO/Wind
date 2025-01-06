#pragma once

#include <cstdint>
#include <atomic>

namespace wind
{
    constexpr uint32_t INVALID_HANDLE = -1;

    template<typename T>
    class Handle
    {
    public:
        static std::atomic<uint32_t> allocateCounter;

        using ValueType = T;

        Handle() = default;
        Handle(uint32_t index) : m_index(index) {}

        bool IsValid() { return m_index != INVALID_HANDLE; }

        operator uint32_t() const { return m_index; }

        virtual void Init() {}
        virtual void Destroy() {}

    protected:
        uint32_t m_index = INVALID_HANDLE;
    };

} // namespace wind