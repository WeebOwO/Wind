#include "Graphics/PSOCache.h"

namespace wind
{
    PSOCache::PSOCache(Device* device) : m_device(device) { Init(); }
    PSOCache::~PSOCache() { Destroy(); }

    void PSOCache::Init() {}
    void PSOCache::Destroy() {}
} // namespace wind