#pragma once

#include <atomic>

namespace crychic
{
    class RefCounted
    {
    public:
        virtual ~RefCounted() = default;

        void IncRefCount() const { ++m_refCount; }
        void DecRefCount() const { --m_refCount; }

        uint32_t GetRefCount() const { return m_refCount.load(); }

    private:
        mutable std::atomic<uint32_t> m_refCount = 0;
    };




} // namespace crychic