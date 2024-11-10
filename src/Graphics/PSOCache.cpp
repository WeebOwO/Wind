#include "Graphics/PSOCache.h"

#include <unordered_map>

#include "Backend/Pipeline.h"
#include "Graphics/ShaderCache.h"

namespace wind
{
    PSOCache::PSOCache(Device* device, ShaderCache* cache) : m_device(device), m_shaderCache(cache) {}
    PSOCache::~PSOCache() {}

    void PSOCache::Init()
    {
        // init pipeline layout
        vk::PipelineLayoutCreateInfo layoutInfo;
        m_defaultLayout = m_device->GetDevice().createPipelineLayout(layoutInfo);

        for (int i = 0; i < PipelineID::Count; ++i)
        {
            CompileToPSO(static_cast<PipelineID>(i));
        }
    }

    void PSOCache::CompileToPSO(PipelineID id)
    {
        PipelineBuilder builder(m_device);

        if (id == PipelineID::Lighting)
        {
            Shader* vs = m_shaderCache->GetShader(ShaderID::VS_Default);
            Shader* ps = m_shaderCache->GetShader(ShaderID::PS_Default);

            std::vector<Shader*> shaders = {vs, ps};

            builder.ApplyDefaultState()
                .SetBlendMode(BlendMode::Opaque)
                .SetShaders(shaders)
                .SetLayout(m_defaultLayout)
                .SetVertexLayout(VertexLayoutType::StaticMesh)
                .SetPassType(PassType::Lighting);

            m_pipelines[id] = builder.Build();
        }
    }

    void PSOCache::Destroy()
    {
        auto vkDevice = m_device->GetDevice();
        vkDevice.destroyPipelineLayout(m_defaultLayout);
        m_pipelines.clear();
    }
} // namespace wind