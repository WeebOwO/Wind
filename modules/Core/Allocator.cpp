#include "Allocator.h"

#include "Core/Log.h"

namespace wind
{
    LinearAllocator::LinearAllocator(size_t size)
    {
        m_Memory = malloc(size);
        m_Size   = size;
    }

    LinearAllocator::~LinearAllocator() { free(m_Memory); }

    void* LinearAllocator::AllocImpl(size_t size, size_t align)
    {
        if (m_Offset + size > m_Size)
        {
            WIND_CORE_ERROR("LinearAllocator out of memory");
            return nullptr;
        }

        void* ptr = static_cast<char*>(m_Memory) + m_Offset;
        m_Offset += size;

        return ptr;
    }
} // namespace wind