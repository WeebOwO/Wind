#pragma once

#include <atomic>

namespace wind
{
    struct RefCounted
    {
    public:
        RefCounted() : m_RefCount(0) {}
        virtual ~RefCounted() = default;

        void AddRef() { ++m_RefCount; }
        void Release()
        {
            if (--m_RefCount == 0)
            {
                delete this;
            }
        }

    private:
        std::atomic<uint32_t> m_RefCount;
    };
} // namespace wind