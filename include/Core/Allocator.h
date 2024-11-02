#pragma once

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

    private:
        virtual void* AllocImpl(size_t size, size_t align) = 0;
    };

    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(size_t size);
        ~LinearAllocator();

    private:
        void* AllocImpl(size_t size, size_t align) override;

        void*  m_memory;
        size_t m_size;
        size_t m_offset;
    };
} // namespace wind