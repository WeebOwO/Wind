#include "ResourcePool.h"

namespace wind
{
    static const uint32_t k_invalid_index = 0xffffffff;

    ResourcePool::ResourcePool(Allocator* allocator, uint32_t poolSize, uint32_t resourceSize) :
        m_allocator(allocator), m_poolSize(poolSize), m_resourceSize(resourceSize)
    {
        size_t allocationoSize = poolSize * (resourceSize + sizeof(uint32_t));
    }
} // namespace wind