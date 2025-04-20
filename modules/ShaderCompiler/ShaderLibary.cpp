#include "ShaderLibary.h"

#include <filesystem>
#include <fstream>

#include "Backend/Device.h"
#include "Core/Log.h"
#include "Core/GlobalContext.h"

namespace
{
    using ShaderPair = std::pair<wind::ShaderID, std::string>;
    static const std::vector<ShaderPair> shaderFileNames {
        {wind::ShaderID::None, ""},
        {wind::ShaderID::Triangle_VS, "Triangle.vert"},
        {wind::ShaderID::Triangle_PS, "Triangle.frag"},
    };

    static std::unordered_map<std::string, wind::ShaderID> shaderReverseMap;

    std::filesystem::path GetShaderEntry(wind::ShaderID id)
    {
        auto shaderPath = wind::g_GlobalContext->pathManager.GetShaderRootDir();

        return shaderPath / shaderFileNames[static_cast<size_t>(id)].second;
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

    void ShaderLibrary::ReleaseShader(ShaderID id)
    {
        if (id >= ShaderID::Count || id == ShaderID::None)
        {
            WIND_CORE_ERROR("Invalid shader id.");
            return;
        }

        m_Shaders[static_cast<size_t>(id)]->ReleaseRHI();
    }

    void ShaderLibrary::CompileShader(ShaderID id)
    {
        if (id >= ShaderID::Count || id == ShaderID::None)
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
        m_Shaders[static_cast<size_t>(id)]->InitRHI();
    }

    void ShaderLibrary::Init(Device* device)
    {
        m_Device = device;

        std::regex pattern(".+\\.vert|.+\\.frag");

        auto shaderPathRoot = wind::g_GlobalContext->pathManager.GetShaderRootDir();

        m_FileWatcher = std::make_unique<FileWatcher>(shaderPathRoot, pattern);
        // construct the reverse map
        for (const auto& [id, name] : shaderFileNames)
        {
            shaderReverseMap[name] = id;
        }

        for (size_t i = 1; i < m_Shaders.size(); i++)
        {
            CompileShader(static_cast<ShaderID>(i));
        }
    }

    void ShaderLibrary::Destroy()
    {
        std::for_each(m_Shaders.begin(), m_Shaders.end(),
         [](auto& shader) {
            if (shader != nullptr)
            {
                shader->ReleaseRHI();
            }
        });
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
                ReleaseShader(id);
                CompileShader(id);
                m_DirtyShaders.insert(id);
            }
        }

        m_FileWatcher->FlushDirtyFiles();
    }

    bool ShaderLibrary::HaveDirtyShaders() const { return !m_DirtyShaders.empty(); }
    void ShaderLibrary::AllDirtyClear() { m_DirtyShaders.clear(); }

    Shader* ShaderLibrary::GetShader(ShaderID id) { return m_Shaders[static_cast<size_t>(id)].get(); }

    Shader* ShaderLibrary::GetShader(const std::string& name)
    {
        if (shaderReverseMap.find(name) == shaderReverseMap.end())
        {
            WIND_CORE_ERROR("Shader {0} not found.", name);
            return nullptr;
        }

        auto id = shaderReverseMap[name];
        return m_Shaders[static_cast<size_t>(id)].get();
    }

    ShaderID ShaderLibrary::GetShaderID(const std::string& name)
    {
        if (shaderReverseMap.find(name) == shaderReverseMap.end())
        {
            WIND_CORE_ERROR("Shader {0} not found.", name);
            return ShaderID::None;
        }

        return shaderReverseMap[name];
    }
} // namespace wind