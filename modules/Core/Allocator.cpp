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

    PoolAllocator::PoolAllocator(size_t size, size_t blockSize, size_t alignment) :
        m_BlockSize(AlignUp(blockSize, alignment)), m_PoolSize(size), m_BlockAlignment(alignment)
    {
        m_PoolStart = operator new(size);

        const auto magicNum = 0xdeadbeef;
        memset(m_PoolStart, magicNum, size);

        m_FreeList = static_cast<FreeBlock*>(m_PoolStart);

        FreeBlock* block = m_FreeList;
        for (size_t i = 0; i < size / blockSize; i++)
        {
            block->next = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(block) + blockSize);
            block       = block->next;
        }

        block->next = nullptr;
    }

    PoolAllocator::~PoolAllocator() { operator delete(m_PoolStart); }

    void* PoolAllocator::AllocImpl(size_t size, size_t align)
    {
        if (size > m_BlockSize)
        {
            WIND_CORE_ERROR("PoolAllocator block size too small");
            return nullptr;
        }

        FreeBlock* block = m_FreeList;
        m_FreeList       = block->next;

        return block;
    }

    void PoolAllocator::FreeImpl(void* ptr)
    {
        FreeBlock* block = static_cast<FreeBlock*>(ptr);
        block->next      = m_FreeList;
        m_FreeList       = block;
    }
} // namespace wind