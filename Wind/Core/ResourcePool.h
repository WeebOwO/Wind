#pragma once

#include "Memory.h"

namespace wind
{
    class ResourcePool
    {
    public:
        ResourcePool(Allocator* allocator, uint32_t poolSize, uint32_t resourceSize);
        ~ResourcePool();

        uint32_t ObTainResource(); // return index

    private:
        uint8_t*   m_byte;
        uint32_t*  m_freeIndices;
        Allocator* m_allocator;
        uint32_t   m_freeIndicesHead = 0;
        uint32_t   m_poolSize        = 16;
        uint32_t   m_resourceSize    = 4;
        uint32_t   m_usedIndices     = 0;
    };
} // namespace wind