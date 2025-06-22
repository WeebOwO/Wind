#include "ShaderLibary.h"

#include <filesystem>
#include <fstream>

#include "Backend/Device.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"
#include "ShaderCompiler/GlslCompiler.h"

namespace
{
    std::string ReadShaderFile(const std::filesystem::path& path)
    {
        std::filesystem::path shaderRootDir = wind::g_GlobalContext->pathManager.GetShaderRootDir();
        std::filesystem::path fullPath      = shaderRootDir / path;

        if (!std::filesystem::exists(fullPath))
        {
            WIND_CORE_ERROR("Shader file does not exist: {}", fullPath.string());
            return "";
        }

        std::ifstream file(fullPath);
        std::string   text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        return text;
    }

    void FillBlobInfo(wind::BlobData& blob, const std::string& fileName)
    {
        if (fileName.ends_with(".vert"))
        {
            blob.stage = vk::ShaderStageFlagBits::eVertex;
        }
        else if (fileName.ends_with(".frag"))
        {
            blob.stage = vk::ShaderStageFlagBits::eFragment;
        }

        auto code = ReadShaderFile(fileName);

        blob.spirv = wind::glsl::CompileGlslToSpv(code, blob.stage);
    }
} // namespace

namespace wind
{
    ShaderLibrary::ShaderLibrary() {}

    ShaderLibrary::~ShaderLibrary() {}

    void ShaderLibrary::Init(Device* device)
    {
        m_Device = device;

        // watch all .slang files
        std::regex pattern(".+\\.slang");

        auto shaderPathRoot = wind::g_GlobalContext->pathManager.GetShaderRootDir();

        m_FileWatcher = std::make_unique<FileWatcher>(shaderPathRoot, pattern);
    }

    void ShaderLibrary::Destroy()
    {
        for (auto& shader : m_TrackedShaders)
        {
            shader->ReleaseRHI();
            shader.reset();
        }
    }

    void ShaderLibrary::Update()
    {
        // todo: support hot reload shader
        return;
    }

    bool ShaderLibrary::HaveDirtyShaders() const { return !m_DirtyShaders.empty(); }
    void ShaderLibrary::AllDirtyClear() { m_DirtyShaders.clear(); }

    ShaderLibrary::ShaderRef ShaderLibrary::CreateShader(BlobData& data)
    {
        auto shader = m_Device->CreateResourceRef<Shader>(data);
        shader->InitRHI();
        m_TrackedShaders.push_back(shader);
        return shader;
    }
} // namespace wind