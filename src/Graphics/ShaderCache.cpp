#include "Graphics/ShaderCache.h"

#include "Backend/Device.h"

namespace wind
{
    ShaderCache::ShaderCache(Device* device) : m_device(device) {}
    ShaderCache::~ShaderCache() {}

    std::string GetShaderPath(ShaderID id)
    {
        switch (id)
        {
            case ShaderID::VS_Default:
                return "shaders/DefaultVS.slang";
            case ShaderID::PS_Default:
                return "shaders/DefaultPS.slang";
            case ShaderID::CS_Default:
                return "shaders/DefaultCS.slang";
            default:
                return "";
        }
    }

    void ShaderCache::CompileShader(ShaderID id)
    {
        Shader* shader = nullptr;

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