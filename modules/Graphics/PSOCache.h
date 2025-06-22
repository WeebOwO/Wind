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

    using CustomPipelineID = int;

    class PipelineManager
    {
    public:
        PipelineManager();
        ~PipelineManager();

        void Init(Device* device, ShaderLibrary* shaderLibrary);
        void Destroy();

        void Update();

        void CompilePipeline(PipelineID id);
        void RecompilePipeline(PipelineID id);

        Pipeline* GetPipeline(PipelineID id) { return m_Pipelines[id].pipeline.get(); }
        Pipeline* GetPipeline(CustomPipelineID id) { return m_CustomPipelines[id].pipeline.get(); }

        CustomPipelineID CreatePipeline(GraphicPipelineDesc& desc);
        ShaderLibrary* GetShaderLibrary() { return m_ShaderLibrary; }

        vk::PipelineLayout GetEmptyPipelineLayout() const { return m_EmptyPipelineLayout; }

    private:
        struct PipelineRecord
        {
            std::vector<ShaderID>     shaderIDs; // shader ids
            std::unique_ptr<Pipeline> pipeline;
        };

        using PreDefinePSOMap = std::unordered_map<PipelineID, PipelineRecord>;
        using CustomPSOMap    = std::unordered_map<CustomPipelineID, PipelineRecord>;

        PreDefinePSOMap m_Pipelines;
        CustomPSOMap    m_CustomPipelines;
        Device*         m_Device;
        ShaderLibrary*  m_ShaderLibrary;

        // pre define pipeline layout
        vk::PipelineLayout m_EmptyPipelineLayout;
    };
} // namespace wind