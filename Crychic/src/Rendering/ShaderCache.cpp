#include "ShaderCache.h"

#include <unordered_map>

#include <RefCntAutoPtr.hpp>

#include "Core/Paths.h"
#include "Loader/BasicIO.h"

namespace crychic
{
    static IRenderDevice* renderDevice = nullptr;

    std::unordered_map<ShaderID, RefCntAutoPtr<IShader>> shaderCache;

    constexpr std::string GetShaderFileName(ShaderID id)
    {
        switch (id)
        {
            case ShaderID::VS_BasePass:
                return "BasePassVS.hlsl";
            case ShaderID::PS_BasePass:
                return "BasePassPS.hlsl";
            default:
                return "";
        }
    }

    SHADER_TYPE GetShaderType(ShaderID id)
    {
        switch (id)
        {
            case ShaderID::VS_BasePass:
                return SHADER_TYPE_VERTEX;
            case ShaderID::PS_BasePass:
                return SHADER_TYPE_PIXEL;
            default:
                return SHADER_TYPE_UNKNOWN;
        }
    }

    void CompileShader(ShaderID id)
    {
        RefCntAutoPtr<IShader> shader;
        ShaderCreateInfo       shaderCI;
        shaderCI.SourceLanguage  = SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.EntryPoint      = "main";
        shaderCI.Desc.ShaderType = GetShaderType(id);

        std::filesystem::path shaderPath   = Paths::ShaderDir() / GetShaderFileName(id);
        std::string           shaderSource = io::LoadFileAsString(shaderPath);

        shaderCI.Source = shaderSource.c_str();

        renderDevice->CreateShader(shaderCI, &shader);

        shaderCache[id] = shader;
    }

    void CompileAllShaders()
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(ShaderID::Count); ++i)
        {
            CompileShader(static_cast<ShaderID>(i));
        }
    }

    IShader* ShaderCache::GetShader(crychic::ShaderID id) { return shaderCache[id]; }

    void ShaderCache::Init(IRenderDevice* device)
    {
        renderDevice = device;
        CompileAllShaders();
    }

    void ShaderCache::Destroy()
    {
        for (auto& shader : shaderCache)
        {
            shader.second = nullptr;
        }
    }
} // namespace crychic