#include "PsoCache.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Backend/Device.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"

using json = nlohmann::json;

namespace
{
    static std::unordered_map<wind::PipelineID, std::string> PipeLineFileNames = {
        {wind::PipelineID::Triangle, "Triangle.json"},
        {wind::PipelineID::UI, "UI.json"},
    };
}

namespace wind
{
    PSOCache::PSOCache() {}
    PSOCache::~PSOCache() {}

    void PSOCache::Init(Device* device, ShaderLibrary* shaderLibrary)
    {
        m_Device        = device;
        m_ShaderLibrary = shaderLibrary;

        for (int i = 0; i < static_cast<int>(PipelineID::Count); i++)
        {
            CompilePipeline(static_cast<PipelineID>(i));
        }
    }

    void PSOCache::Destroy()
    {
        for (auto& [id, pipeline] : m_Pipelines)
        {
            pipeline.pipeline->ReleaseRHI();
        }
    }

    void PSOCache::Update()
    {
        auto& dirtyShaders = m_ShaderLibrary->GetDirtyShaders();

        for (const auto& id : dirtyShaders)
        {
            for (auto& [pipelineID, pipeline] : m_Pipelines)
            {
                if (std::find(pipeline.shaderIDs.begin(), pipeline.shaderIDs.end(), id) != pipeline.shaderIDs.end())
                {
                    RecompilePipeline(pipelineID);
                }
            }
        }
    }

    void PSOCache::RecompilePipeline(PipelineID id)
    {
        m_Pipelines[id].pipeline->ReleaseRHI();
        CompilePipeline(id);
    }

    void PSOCache::CompilePipeline(PipelineID id)
    {
        std::filesystem::path path = g_GlobalContext->pathManager.GetShaderRootDir() / PipeLineFileNames[id];
        std::ifstream         file(path);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", path.string());
        }

        json data = json::parse(file);

        GraphicPipelineDesc desc;

        std::string vertexShaderPath, fragmentShaderPath;

        for (const auto& elem : data.items())
        {
            std::string key   = elem.key();
            json        value = elem.value();

            if (key == "shaders")
            {
                if (value.is_object())
                {
                    for (const auto& shader : value.items())
                    {
                        std::string shaderType = shader.key();
                        std::string shaderPath = shader.value().get<std::string>();

                        if (shaderType == "vertex")
                        {
                            vertexShaderPath = shaderPath;
                        }
                        else if (shaderType == "fragment")
                        {
                            fragmentShaderPath = shaderPath;
                        }
                        else
                        {
                            WIND_CORE_ERROR("Unsupported shader type: {}", shaderType);
                        }
                    }
                }
            }
        }

        vk::PipelineLayout pipelineLayout = m_Device->GetDevice().createPipelineLayout({});

        desc.SetShaders(m_ShaderLibrary->GetShader(vertexShaderPath)->GetBlobData().module,
                        m_ShaderLibrary->GetShader(fragmentShaderPath)->GetBlobData().module)
            .SetLayout(pipelineLayout)
            .SetInputTopology(vk::PrimitiveTopology::eTriangleList)
            .SetPolygonMode(vk::PolygonMode::eFill)
            .SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise)
            .SetMultisamplingNone()
            .EnableDepthTest(true, vk::CompareOp::eLessOrEqual);

        m_Pipelines[id].pipeline = m_Device->CreateResourceUnique<Pipeline>(desc);
        m_Pipelines[id].pipeline->InitRHI();

        m_Pipelines[id].shaderIDs.clear();
        m_Pipelines[id].shaderIDs.push_back(vertexShaderPath);
        m_Pipelines[id].shaderIDs.push_back(fragmentShaderPath);

        file.close();
    }

} // namespace wind