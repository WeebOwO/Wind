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

    ShaderInfo GetShaderPath(ShaderID id)
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

        auto getShaderType = [](ShaderID id) -> ShaderType {
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
        };

        auto getShaderStage = [](ShaderType type) -> vk::ShaderStageFlagBits {
            switch (type)
            {
                case ShaderType::Vertex:
                    return vk::ShaderStageFlagBits::eVertex;
                case ShaderType::Fragment:
                    return vk::ShaderStageFlagBits::eFragment;
                case ShaderType::Compute:
                    return vk::ShaderStageFlagBits::eCompute;
                default:
                    return vk::ShaderStageFlagBits::eVertex;
            }
        };

        ShaderInfo info {.filePath = shaderRootDir / getShaderName(id), .type = getShaderType(id)};

        return info;
    }

    void ShaderCache::CompileShader(ShaderID id)
    {
        ShaderInfo info = GetShaderPath(id);
        BlobData   blob(io::ReadFileAsString(info.filePath.string()), info.type);

        std::shared_ptr<Shader> shader = std::make_shared<Shader>(m_device, blob);
        shader->Init();

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
} // namespace wind