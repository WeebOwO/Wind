#pragma once

#include <unordered_set>
#include <vector>

#include "Backend/Shader.h"
#include "Core/FileWatcher.h"

namespace wind
{
    class Device;

    enum class ShaderID
    {
        Triangle_VS,
        Triangle_PS,
        Count
    };

    class ShaderLibrary
    {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

        Shader* GetShader(ShaderID id) { return m_Shaders[static_cast<size_t>(id)].get(); }
        void    Init(Device* device);
        void    Destroy();

        void Update();

        bool        HaveDirtyShaders() const;
        const auto& GetDirtyShaders() const { return m_DirtyShaders; }
        void        AllDirtyClear();

    private:
        void CompileShader(ShaderID id);

        Device*                              m_Device;
        std::vector<std::unique_ptr<Shader>> m_Shaders;
        std::unordered_set<ShaderID>         m_DirtyShaders;
        std::unique_ptr<FileWatcher>         m_FileWatcher;
    };
} // namespace wind