#pragma once

#include <concepts>

#include "RefCount.h"

namespace wind
{
    template<typename T>
    requires std::derived_from<T, RefCounted> 
    class Shared
    {
    public:
        Shared() noexcept : m_Ptr(nullptr) {}

        explicit Shared(T* ptr) noexcept : m_Ptr(ptr)
        {
            if (m_Ptr)
                m_Ptr->AddRef();
        }

        Shared(const Shared& other) noexcept : m_Ptr(other.m_Ptr)
        {
            if (m_Ptr)
                m_Ptr->AddRef();
        }

        Shared(Shared&& other) noexcept : m_Ptr(other.m_Ptr) { other.m_Ptr = nullptr; }

        ~Shared()
        {
            if (m_Ptr)
                m_Ptr->Release();
        }

        Shared& operator=(const Shared& other) noexcept
        {
            if (this != &other)
            {
                if (m_Ptr)
                    m_Ptr->Release();
                m_Ptr = other.m_Ptr;
                if (m_Ptr)
                    m_Ptr->AddRef();
            }
            return *this;
        }

        Shared& operator=(Shared&& other) noexcept
        {
            if (this != &other)
            {
                if (m_Ptr)
                    m_Ptr->Release();
                m_Ptr       = other.m_Ptr;
                other.m_Ptr = nullptr;
            }
            return *this;
        }

        T*       get() const noexcept { return m_Ptr; }
        T&       operator*() const noexcept { return *m_Ptr; }
        T*       operator->() const noexcept { return m_Ptr; }
        explicit operator bool() const noexcept { return m_Ptr != nullptr; }

    private:
        T* m_Ptr;
    };

    template<typename T, typename... Args>
    Shared<T> MakeShared(Args&&... args)
    {
        return MakeShared<T>(new T(std::forward<Args>(args)...));
    }
} // namespace wind