#pragma once

#include <cstddef>

namespace wind
{
    class Allocator
    {
    public:
        template<typename T>
        T* Allocate(size_t count, size_t align = alignof(T))
        {
            return static_cast<T*>(AllocImpl(sizeof(T) * count, align));
        }

        void Free(void* ptr) { FreeImpl(ptr); }

    private:
        virtual void* AllocImpl(size_t size, size_t align) = 0;
        virtual void  FreeImpl(void* ptr)                  = 0;
    };

    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(size_t size);
        ~LinearAllocator();

    private:
        void* AllocImpl(size_t size, size_t align) override;
        void  FreeImpl(void* ptr) override {}

        void*  m_Memory;
        size_t m_Size;
        size_t m_Offset;
    };

    struct PoolAllocator : public Allocator
    {
        PoolAllocator(size_t size, size_t blockSize, size_t alignment = alignof(std::max_align_t));
        ~PoolAllocator();

    private:
        void* AllocImpl(size_t size, size_t align) override;
        void  FreeImpl(void* ptr);

        size_t AlignUp(size_t value, size_t alignment) { return (value + (alignment - 1)) & ~(alignment - 1); }

        struct FreeBlock
        {
            FreeBlock* next;
        };

        void*      m_PoolStart;
        FreeBlock* m_FreeList;
        size_t     m_BlockSize;
        size_t     m_PoolSize;
        size_t     m_BlockAlignment;
    };
} // namespace wind