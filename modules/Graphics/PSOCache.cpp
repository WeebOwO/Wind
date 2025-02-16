#include "PsoCache.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "Backend/Device.h"
#include "Core/Log.h"
#include "Core/Path.h"

using json = nlohmann::json;

namespace
{
    static std::unordered_map<wind::PipelineID, std::string> PipeLineFileNames = {
        {wind::PipelineID::Triangle, "Triangle.json"},
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
            pipeline->ReleaseRHI();
        }
    }

    void PSOCache::Update()
    {
        // update all the dirty pipelines
    }

    void PSOCache::CompilePipeline(PipelineID id)
    {
        std::filesystem::path path = path::GetShaderRootDir() / PipeLineFileNames[id];
        std::ifstream         file(path);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", path.string());
        }

        json data = json::parse(file);

        GraphicPipelineDesc desc;
        Shader*             vertexShader;
        Shader*             fragmentShader;

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
                            vertexShader = m_ShaderLibrary->GetShader(shaderPath);
                        }
                        else if (shaderType == "fragment")
                        {
                            fragmentShader = m_ShaderLibrary->GetShader(shaderPath);
                        }
                        else
                        {
                            WIND_CORE_ERROR("Unsupported shader type.");
                        }
                    }
                }
            }
        }

        assert(vertexShader != nullptr && fragmentShader != nullptr);

        vk::PipelineLayout pipelineLayout = m_Device->GetDevice().createPipelineLayout({});

        desc.SetShaders(vertexShader->GetBlobData().module, fragmentShader->GetBlobData().module)
            .SetLayout(pipelineLayout)
            .SetInputTopology(vk::PrimitiveTopology::eTriangleList)
            .SetPolygonMode(vk::PolygonMode::eFill)
            .SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise)
            .SetMultisamplingNone()
            .EnableDepthTest(true, vk::CompareOp::eLessOrEqual);

        m_Pipelines[id] = m_Device->CreateResourceUnique<Pipeline>(desc);

        file.close();

        m_Pipelines[id]->InitRHI();
    }

} // namespace wind