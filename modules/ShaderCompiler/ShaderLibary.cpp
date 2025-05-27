#include "ShaderLibary.h"

#include <filesystem>
#include <fstream>

#include "Backend/Device.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"

namespace
{
    std::string ReadShaderFile(const std::filesystem::path& path)
    {
        std::filesystem::path shaderRootDir = wind::g_GlobalContext->pathManager.GetShaderRootDir();
        std::filesystem::path fullPath = shaderRootDir / path;

        if (!std::filesystem::exists(fullPath))
        {
            WIND_CORE_ERROR("Shader file does not exist: {}", fullPath.string());
            return "";
        }

        std::ifstream file(fullPath);
        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        return text;
    }

    void FillShaderType(wind::BlobData& blob, const std::string& fileName)
    {
        if (fileName.ends_with(".vert"))
        {
            blob.type  = wind::ShaderType::Vertex;
            blob.stage = vk::ShaderStageFlagBits::eVertex;
        }
        else if (fileName.ends_with(".frag"))
        {
            blob.type  = wind::ShaderType::Fragment;
            blob.stage = vk::ShaderStageFlagBits::eFragment;
        }
        else if (fileName.ends_with(".comp"))
        {
            blob.type  = wind::ShaderType::Compute;
            blob.stage = vk::ShaderStageFlagBits::eCompute;
        }
    }
} // namespace

namespace wind
{
    ShaderLibrary::ShaderLibrary() {}

    ShaderLibrary::~ShaderLibrary() {}

    void ShaderLibrary::CompileShader(ShaderID& id)
    {
        auto path = id.fileName;
        auto code = ReadShaderFile(path);

        BlobData blob;
        blob.shaderCode = code;
        FillShaderType(blob, path);

        m_Shaders[id] = m_Device->CreateResourceUnique<Shader>(blob);
        m_Shaders[id]->InitRHI();
    }

    void ShaderLibrary::Init(Device* device)
    {
        m_Device = device;

        std::regex pattern(".+\\.vert|.+\\.frag");

        auto shaderPathRoot = wind::g_GlobalContext->pathManager.GetShaderRootDir();

        m_FileWatcher = std::make_unique<FileWatcher>(shaderPathRoot, pattern);
    }

    void ShaderLibrary::Destroy()
    {
        std::for_each(m_Shaders.begin(), m_Shaders.end(), [](auto& shader) 
        {
            shader.second->ReleaseRHI();
            shader.second.reset();
        });
    }

    void ShaderLibrary::Update()
    {
        return;
        m_FileWatcher->Update();
        for (const auto& [name, status] : m_FileWatcher->GetWatchedFiles())
        {
            if (status.isDirty)
            {
                // auto     name = status.path.filename().string();
                // ShaderID id   = shaderReverseMap[name];
                // WIND_CORE_INFO("Recompile: {0}", name);
                // ReleaseShader(id);
                // CompileShader(id);
                // m_DirtyShaders.insert(id);
            }
        }

        m_FileWatcher->FlushDirtyFiles();
    }

    bool ShaderLibrary::HaveDirtyShaders() const { return !m_DirtyShaders.empty(); }
    void ShaderLibrary::AllDirtyClear() { m_DirtyShaders.clear(); }

    Shader* ShaderLibrary::GetShader(const std::string& filePath)
    {
        ShaderID id(filePath);
        auto it = m_Shaders.find(id);

        if (it == m_Shaders.end())
        {
            // If shader not found, compile it
            CompileShader(id);
        }

        return m_Shaders[id].get();
    }
} // namespace wind