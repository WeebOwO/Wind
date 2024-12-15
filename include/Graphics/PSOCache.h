#pragma once

#include "Backend/Device.h"
#include "Backend/Enum.h"
#include "Backend/Pipeline.h"
#include "Core/UUID.h"

namespace wind
{
    class ShaderCache;
    class PSOCache;

    // store predefined pipeline id
    enum PipelineID : uint32_t
    {
        Lighting = 0,
        Count
    };

    // precompiled pso
    class PSOCache
    {
    public:
        using PipelineMaps = std::unordered_map<PipelineID, std::shared_ptr<Pipeline>>;

        PSOCache(Device* device, ShaderCache* shaderCache);
        ~PSOCache();

        void Init();
        void Destroy();

        // todo: add this to material system
        void GetPipelineByUUID(UUID uuid) const;
        auto GetPipeline(PipelineID id) const { return m_Pipelines.at(id); }

    private:
        void CompileToPSO(PipelineID id);
        void GetShaderInfo(ShadingModel shadingModel);

        Device*            m_Device;
        ShaderCache*       m_ShaderCache;
        PipelineMaps       m_Pipelines;
        vk::PipelineLayout m_DefaultLayout;
    };
} // namespace wind