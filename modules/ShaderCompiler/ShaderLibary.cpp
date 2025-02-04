#include "ShaderLibary.h"

#include <filesystem>
#include <fstream>

#include "Backend/Device.h"
#include "Core/Log.h"
#include "Core/Path.h"

namespace
{
    using ShaderPair = std::pair<wind::ShaderID, std::string>;
    static const std::vector<ShaderPair> shaderFileNames {
        {wind::ShaderID::Triangle_VS, "Triangle.vert"},
        {wind::ShaderID::Triangle_PS, "Triangle.frag"},
    };

    static std::unordered_map<std::string, wind::ShaderID> shaderReverseMap;

    std::filesystem::path GetShaderEntry(wind::ShaderID id)
    {
        return wind::path::GetShaderRootDir() / shaderFileNames[static_cast<size_t>(id)].second;
    }

    std::string ReadShaderFile(const std::filesystem::path& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", path.string());
        }

        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        return text;
    }
} // namespace

namespace wind
{
    ShaderLibrary::ShaderLibrary() { m_Shaders.resize(static_cast<size_t>(ShaderID::Count)); }

    ShaderLibrary::~ShaderLibrary() {}

    void ShaderLibrary::CompileShader(ShaderID id)
    {
        if (id >= ShaderID::Count)
        {
            WIND_CORE_ERROR("Invalid shader id.");
            return;
        }

        auto path = GetShaderEntry(id);
        auto code = ReadShaderFile(path);

        BlobData blob;
        blob.shaderCode = code;

        switch (id)
        {
            case ShaderID::Triangle_VS:
                blob.type = ShaderType::Vertex;
                break;
            case ShaderID::Triangle_PS:
                blob.type = ShaderType::Fragment;
                break;
            default:
                WIND_CORE_ERROR("Unsupported shader type.");
                break;
        }

        m_Shaders[static_cast<size_t>(id)] = m_Device->CreateResourceUnique<Shader>(blob);
    }

    void ShaderLibrary::Init(Device* device)
    {
        m_Device = device;

        std::regex pattern(".+\\.vert|.+\\.frag");
        m_FileWatcher = std::make_unique<FileWatcher>(wind::path::GetShaderRootDir(), pattern);
        // construct the reverse map
        for (const auto& [id, name] : shaderFileNames)
        {
            shaderReverseMap[name] = id;
        }

        for (size_t i = 0; i < m_Shaders.size(); i++)
        {
            CompileShader(static_cast<ShaderID>(i));
        }

        for (auto& shader : m_Shaders)
        {
            shader->InitRHI();
        }
    }

    void ShaderLibrary::Destroy()
    {
        for (auto& shader : m_Shaders)
        {
            shader->ReleaseRHI();
        }
    }

    void ShaderLibrary::Update()
    {
        m_FileWatcher->Update();
        for (const auto& [name, status] : m_FileWatcher->GetWatchedFiles())
        {
            if (status.isDirty)
            {
                auto     name = status.path.filename().string();
                ShaderID id   = shaderReverseMap[name];
                WIND_CORE_INFO("Recompile: {0}", name);
                m_Shaders[static_cast<size_t>(id)]->ReleaseRHI();
                CompileShader(id);
                m_Shaders[static_cast<size_t>(id)]->InitRHI();
                m_DirtyShaders.insert(id);
            }
        }

        m_FileWatcher->FlushDirtyFiles();
    }

    bool ShaderLibrary::HaveDirtyShaders() const { return !m_DirtyShaders.empty(); }
    void ShaderLibrary::AllDirtyClear() { m_DirtyShaders.clear(); }
} // namespace wind