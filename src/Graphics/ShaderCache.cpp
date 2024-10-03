#include "Graphics/ShaderCache.h"

#include "Backend/Device.h"
#include "Core/Path.h"

namespace wind
{
    ShaderCache::ShaderCache(Device* device) : m_device(device) {}
    ShaderCache::~ShaderCache() {}

    std::filesystem::path GetShaderPath(ShaderID id)
    {
        std::filesystem::path shaderRootDir = path::GetShaderRootDir();

        auto getShaderName = [](ShaderID id) -> std::string {
            switch (id)
            {
                case ShaderID::VS_Default:
                    return "Triangle.vert";
                case ShaderID::PS_Default:
                    return "Triangle.frag";
                default:
                    return "";
            }
        };

        return shaderRootDir / getShaderName(id);
    }

    void ShaderCache::CompileShader(ShaderID id)
    {
        Shader* shader = nullptr;
        std::filesystem::path shaderPath = GetShaderPath(id);
        m_shaders[id] = shader;
    }

    void ShaderCache::Init()
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(ShaderID::Count); ++i)
        {
            CompileShader(static_cast<ShaderID>(i));
        }
    }

    void ShaderCache::Destroy() {}
} // namespace wind