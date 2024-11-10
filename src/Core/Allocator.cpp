#include "Core/Allocator.h"

#include "Core/Log.h"

namespace wind
{
    LinearAllocator::LinearAllocator(size_t size)
    {
        m_memory = malloc(size);
        m_size   = size;
    }

    LinearAllocator::~LinearAllocator() { free(m_memory); }

    void* LinearAllocator::AllocImpl(size_t size, size_t align)
    {
        if (m_offset + size > m_size)
        {
            WIND_CORE_ERROR("LinearAllocator out of memory");
            return nullptr;
        }

        void* ptr = static_cast<char*>(m_memory) + m_offset;
        m_offset += size;

        return ptr;
    }
} // namespace wind