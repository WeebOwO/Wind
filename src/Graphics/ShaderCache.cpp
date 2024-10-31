#include "Graphics/ShaderCache.h"

#include "Backend/Device.h"
#include "Backend/Enum.h"
#include "Backend/Shader.h"
#include "Core/IO.h"
#include "Core/Path.h"
#include <cstdint>

namespace wind
{
    ShaderCache::ShaderCache(Device* device) : m_device(device) {}
    ShaderCache::~ShaderCache() {}

    struct ShaderInfo
    {
        std::filesystem::path filePath;
        ShaderType            type;
    };

    std::string GetShaderName(ShaderID id)
    {
        switch (id)
        {
            case ShaderID::VS_Default:
                return "Triangle.vert";
            case ShaderID::PS_Default:
                return "Triangle.frag";
            default:
                return "";
        };
    }

    ShaderType GetShaderType(ShaderID id)
    {
        if (id < ShaderID::PS_Default)
        {
            return ShaderType::Vertex;
        }
        else if (id < ShaderID::CS_Default)
        {
            return ShaderType::Fragment;
        }
        else
        {
            return ShaderType::Compute;
        }
    }

    ShaderInfo GetShaderPath(ShaderID id)
    {
        std::filesystem::path shaderRootDir = path::GetShaderRootDir();

        ShaderInfo info {.filePath = shaderRootDir / GetShaderName(id), .type = GetShaderType(id)};

        return info;
    }

    void ShaderCache::CompileShader(ShaderID id)
    {
        ShaderInfo info = GetShaderPath(id);
        BlobData   blob(io::ReadFileAsString(info.filePath.string()), info.type);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(m_device, blob);

        m_shaders[id] = shader;
    }

    void ShaderCache::Init()
    {
        // not need to compile compute shader for now
        for (uint32_t i = 0; i < static_cast<uint32_t>(ShaderID::CS_Default); ++i)
        {
            CompileShader(static_cast<ShaderID>(i));
        }
    }

    void ShaderCache::Destroy() {}

    Shader* ShaderCache::GetShader(ShaderID id) { return m_shaders[id].get(); }
} // namespace wind