#pragma once

#include "Backend/Pipeline.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;

    // buildin pipeline id
    enum class PipelineID : uint32_t
    {
        Triangle,
        UI,
        Count
    };

    class PSOCache
    {
    public:
        PSOCache();
        ~PSOCache();

        void Init(Device* device, ShaderLibrary* shaderLibrary);
        void Destroy();

        void Update();

        void CompilePipeline(PipelineID id);
        void RecompilePipeline(PipelineID id);

        Pipeline* GetPipeline(PipelineID id) { return m_Pipelines[id].pipeline.get(); }

    private:
        struct PipelineRecord
        {
            std::vector<ShaderID>     shaderIDs; // shader ids
            std::unique_ptr<Pipeline> pipeline;
        };

        using PSOMap = std::unordered_map<PipelineID, PipelineRecord>;

        PSOMap         m_Pipelines;
        Device*        m_Device;
        ShaderLibrary* m_ShaderLibrary;
    };
} // namespace wind