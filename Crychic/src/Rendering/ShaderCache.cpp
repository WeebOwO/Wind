#include "ShaderCache.h"

namespace crychic
{
    IRenderDevice* renderDevice = nullptr;

    void ShaderCache::Init(IRenderDevice* device) { renderDevice = device; }
    void ShaderCache::Destroy() {}
} // namespace crychic