#pragma once

#include "Backend/Device.h"
#include "Backend/Enum.h"
#include "Backend/Pipeline.h"

namespace wind
{
    class ShaderCache;
    class PSOCache;

    enum PipelineID : uint32_t
    {
        Lighting = 0,
        Count
    };

    // precompiled pso
    class PSOCache
    {
    public:
        PSOCache(Device* device, ShaderCache* shaderCache);
        ~PSOCache();

        void Init();
        void Destroy();

    private:
        void CompileToPSO(PipelineID id);
        void GetShaderInfo(ShadingModel shadingModel);

        Device*                                                   m_device;
        ShaderCache*                                              m_shaderCache;
        std::unordered_map<PipelineID, std::shared_ptr<Pipeline>> m_pipelines;
        vk::PipelineLayout                                        m_defaultLayout;
    };
} // namespace wind