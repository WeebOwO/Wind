#pragma once

#include <unordered_set>
#include <vector>

#include "Backend/Shader.h"
#include "Core/FileWatcher.h"

namespace wind
{
    class Device;

    struct ShaderID
    {
        std::string fileName;
        size_t      hash;

        ShaderID() : fileName(""), hash(0) {}
        ShaderID(const std::string& name) : fileName(name), hash(std::hash<std::string>()(name)) {}

        bool operator==(const ShaderID& other) const { return hash == other.hash; }
        bool operator!=(const ShaderID& other) const { return !(*this == other); }
    };
} // namespace wind

// Provide std::hash specialization for ShaderID
namespace std
{
    template<>
    struct hash<wind::ShaderID>
    {
        std::size_t operator()(const wind::ShaderID& id) const noexcept { return id.hash; }
    };
} // namespace std

namespace wind
{
    class ShaderLibrary
    {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

        Shader* LoadShaderWithFileName(const std::string& fileName);

        void Init(Device* device);
        void Destroy();

        void Update();

        // if the shader is not found, it will create a new shader from this filename
        // this path should be relative to the shader root directory
        Shader* GetShader(const std::string& filePath);

        bool        HaveDirtyShaders() const;
        const auto& GetDirtyShaders() const { return m_DirtyShaders; }
        void        AllDirtyClear();

    private:
        using ShaderMap = std::unordered_map<ShaderID, std::unique_ptr<Shader>>;

        void CompileShader(ShaderID& id);

        Device*                      m_Device;
        ShaderMap                    m_Shaders;
        std::unordered_set<ShaderID> m_DirtyShaders;
        std::unique_ptr<FileWatcher> m_FileWatcher;
    };
} // namespace wind
