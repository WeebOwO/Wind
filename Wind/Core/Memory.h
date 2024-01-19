#pragma once

#include "std.h"

namespace wind
{
    // copy from raptor engine hh
    void MemoryCopy(void* dest, void* source, size_t size) noexcept;
    void MemoryAlign(size_t size, size_t align) noexcept;

    struct MemoryStatistics
    {
        size_t   allocatedBytes;
        size_t   totalBytes;
        uint32_t allocationCount;

        void Add(size_t a) noexcept
        {
            if (a)
            {
                allocatedBytes += a;
                ++allocationCount;
            }
        };
    };

    struct Allocator
    {
        virtual ~Allocator() noexcept {}
        virtual void* allocate(size_t size, size_t alignment)                                    = 0;
        virtual void* allocate(size_t size, size_t alignment, const std::string& file, int line) = 0;

        virtual void deallocate(void* pointer) = 0;
    };

    class HeapAllocator : public Allocator
    {
    public:
        
    private:
        
    };

} // namespace wind