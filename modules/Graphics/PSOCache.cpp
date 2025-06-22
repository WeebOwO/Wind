#include "PsoCache.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Backend/Device.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"

using json = nlohmann::json;

namespace
{
    // clang-format off
    static std::unordered_map<wind::PipelineID, std::string> PipeLineFileNames = 
    {
        {wind::PipelineID::Triangle, "Triangle.json"},
    };
    // clang-format on
} // namespace

namespace wind
{
    PipelineManager::PipelineManager() {}
    PipelineManager::~PipelineManager() {}

    void PipelineManager::Init(Device* device, ShaderLibrary* shaderLibrary)
    {
        m_Device              = device;
        m_ShaderLibrary       = shaderLibrary;
        m_EmptyPipelineLayout = m_Device->GetDevice().createPipelineLayout({});
    }

    void PipelineManager::Destroy()
    {
        for (auto& [id, pipeline] : m_CustomPipelines)
        {
            pipeline.pipeline->ReleaseRHI();
        }
    }

    void PipelineManager::Update()
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

    void PipelineManager::RecompilePipeline(PipelineID id)
    {
        m_Pipelines[id].pipeline->ReleaseRHI();
        CompilePipeline(id);
    }

    void PipelineManager::CompilePipeline(PipelineID id)
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
    }

    CustomPipelineID PipelineManager::CreatePipeline(GraphicPipelineDesc& desc)
    {
        auto id                        = m_CustomPipelines.size();
        m_CustomPipelines[id].pipeline = m_Device->CreateResourceUnique<Pipeline>(desc);
        m_CustomPipelines[id].pipeline->InitRHI();
        return id;
    }

} // namespace wind