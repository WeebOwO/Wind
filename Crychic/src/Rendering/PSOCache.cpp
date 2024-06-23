#include "PSOCache.h"

#include <memory>
#include <unordered_map>

#include <PipelineState.h>
#include <RefCntAutoPtr.hpp>

namespace crychic
{
    IRenderDevice*                                                  renderDevice = nullptr;
    std::unordered_map<PsoStateID, std::unique_ptr<IPipelineState>> psoCache;

    static void CreateAllPso()
    {

    }

    void PSOCache::Init(IRenderDevice* device)
    {
        renderDevice = device;
    }

    void PSOCache::Destroy() {}
} // namespace crychic