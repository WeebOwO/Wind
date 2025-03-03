#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace wind
{
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
        LinearAllocator(size_t size) : m_Size(size) 
        {
            m_Memory = malloc(size);
        }

        ~LinearAllocator() override 
        {
            free(m_Memory);
        }

        void* Allocate(size_t size, size_t alignment) override 
        {
            // detect overflow
            if (m_Offset + size > m_Size)
            {
                return nullptr;
            }

            // align the memory
            std::uintptr_t raw     = reinterpret_cast<std::uintptr_t>(m_Memory) + m_Offset;
            std::uintptr_t aligned = (raw + alignment - 1) & ~(alignment - 1);
            m_Offset               = aligned - reinterpret_cast<std::uintptr_t>(m_Memory) + size;
            return reinterpret_cast<void*>(aligned);
        }
        
        template<typename T>
        T* Allocate(size_t count = 1)
        {
            return reinterpret_cast<T*>(Allocate(sizeof(T) * count, alignof(T)));
        }

        void Free(void* ptr) override 
        {
            // do nothing
        }

    private:
        size_t m_Size;
        void*  m_Memory;
        size_t m_Offset = 0;
    };
} // namespace wind