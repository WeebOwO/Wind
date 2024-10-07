#pragma once

namespace wind
{
    class Allocator
    {
    public:
        template<typename T>
        T* Allocate(size_t count, size_t align = alignof(T))
        {
            return static_cast<T*>(Allocate(sizeof(T) * count));
        }

    private:
        virtual void* Allocate(size_t size) = 0;
    };

    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(size_t size);
        ~LinearAllocator();

        
    private:
        void*  m_memory;
        size_t m_size;
    };
} // namespace wind