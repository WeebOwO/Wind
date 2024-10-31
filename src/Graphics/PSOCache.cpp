#include "Graphics/PSOCache.h"

#include <unordered_map>

#include "Backend/Pipeline.h"
#include "Graphics/ShaderCache.h"

namespace wind
{
    std::unordered_map<PipelineID, std::shared_ptr<Pipeline>> s_pipelines;

    PSOCache::PSOCache(Device* device, ShaderCache* cache) : m_device(device), m_shaderCache(cache) { Init(); }
    PSOCache::~PSOCache() { Destroy(); }

    void PSOCache::Init()
    {
        for (int i = 0; i < PipelineID::Count; ++i)
        {
            CompileToPSO(static_cast<PipelineID>(i));
        }
    }

    void PSOCache::CompileToPSO(PipelineID id)
    {
        switch (id)
        {
            case PipelineID::Lighting: {
                break;
            }
        }
    }

    void PSOCache::Destroy() { s_pipelines.clear(); }
} // namespace wind