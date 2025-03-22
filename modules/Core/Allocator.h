#pragma once

#include <cstddef>
#include <cstdlib>

namespace wind
{
    inline const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment)
    {
        const std::size_t multiplier     = (baseAddress / alignment) + 1;
        const std::size_t alignedAddress = multiplier * alignment;
        const std::size_t padding        = alignedAddress - baseAddress;
        return padding;
    }

    class Allocator
    {
    public:
        Allocator()          = default;
        virtual ~Allocator() = default;

        virtual void* Allocate(size_t size, size_t alignment) = 0;
        virtual void  Free(void* ptr)                         = 0;
    };

    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(size_t size) : m_Size(size), m_Offset(0), m_StartPtr(nullptr) { m_StartPtr = malloc(size); }

        ~LinearAllocator() override { free(m_StartPtr); }

        void* Allocate(size_t size, size_t alignment) override;

        template<typename T>
        T* Allocate(size_t count = 1)
        {
            return reinterpret_cast<T*>(Allocate(sizeof(T) * count, alignof(T)));
        }

        void Free(void* ptr) override;

        void Reset();

    private:
        void*  m_StartPtr;
        size_t m_Size;
        size_t m_Offset;
    };
} // namespace wind