#pragma once

#include <cstdint>
#include <unordered_map>

#include "Backend/Shader.h"

namespace wind
{
    class Device;

    enum class ShaderID : uint32_t
    {
        VS_Default = 0,
        PS_Default,
        CS_Default,
        Count
    };

    class ShaderCache
    {
    public:
        ShaderCache(Device* device);
        ~ShaderCache();

        void Init();
        void Destroy();

        template<typename T>
        T* GetShader(ShaderID id) requires std::derived_from<T, Shader>
        {
            auto it = m_shaders.find(id);
            if (it == m_shaders.end())
            {
                CompileShader(id);
                it = m_shaders.find(id);
            }

            return static_cast<T*>(it->second);
        }

    private:
        using ShaderMap = std::unordered_map<ShaderID, Shader*>;

        void CompileShader(ShaderID id);

        Device*   m_device;
        ShaderMap m_shaders;
    };
} // namespace wind