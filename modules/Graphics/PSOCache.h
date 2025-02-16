#pragma once

#include "Backend/Pipeline.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    class Device;

    // buildin pipeline id
    enum class PipelineID
    {
        Triangle,
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

        Pipeline* GetPipeline(PipelineID id) { return m_Pipelines[id].get(); }

    private:
        using PSOMap = std::unordered_map<PipelineID, std::unique_ptr<Pipeline>>;

        PSOMap         m_Pipelines;
        Device*        m_Device;
        ShaderLibrary* m_ShaderLibrary;
    };
} // namespace wind