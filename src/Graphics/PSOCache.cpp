#include "Graphics/PSOCache.h"

#include <unordered_map>

#include "Backend/Pipeline.h"
#include "Graphics/ShaderCache.h"

namespace wind
{
    PSOCache::PSOCache(Device* device, ShaderCache* cache) : m_Device(device), m_ShaderCache(cache) {}
    PSOCache::~PSOCache() {}

    void PSOCache::Init()
    {
        // init pipeline layout
        vk::PipelineLayoutCreateInfo layoutInfo;
        m_DefaultLayout = m_Device->GetDevice().createPipelineLayout(layoutInfo);

        for (int i = 0; i < PipelineID::Count; ++i)
        {
            CompileToPSO(static_cast<PipelineID>(i));
        }
    }

    void PSOCache::CompileToPSO(PipelineID id)
    {
        PipelineBuilder builder(m_Device);

        if (id == PipelineID::Lighting)
        {
            Shader* vs = m_ShaderCache->GetShader(ShaderID::VS_Default);
            Shader* ps = m_ShaderCache->GetShader(ShaderID::PS_Default);

            std::vector<Shader*> shaders = {vs, ps};

            builder.ApplyDefaultState()
                .SetBlendMode(BlendMode::Opaque)
                .SetShaders(shaders)
                .SetLayout(m_DefaultLayout)
                .SetVertexLayout(VertexLayoutType::StaticMesh)
                .SetPassType(PassType::Lighting);

            m_Pipelines[id] = builder.Build();
        }
    }

    void PSOCache::Destroy()
    {
        auto vkDevice = m_Device->GetDevice();
        vkDevice.destroyPipelineLayout(m_DefaultLayout);
        m_Pipelines.clear();
    }
} // namespace wind