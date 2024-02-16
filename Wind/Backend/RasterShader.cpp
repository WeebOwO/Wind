#include "RasterShader.h"

#include <fstream>
#include <shaderc/shaderc.hpp>

#include "Device.h"
#include "Resource/Loader.h"
#include "Core/PathManager.h"

namespace wind
{

    RasterShader::RasterShader(GPUDevice&              device,
                               const std::string&      name,
                               const vk::ShaderModule& vertexModule,
                               const vk::ShaderModule& fragShaderModule) noexcept :
        Shader(device),
        m_vertexModule(vertexModule), m_fragModule(fragShaderModule)
    {
        SetShaderName(name);
    }

    RasterShader::RasterShader(GPUDevice&         device,
                               const std::string& name,
                               const std::string& vetexShaderName,
                               const std::string& fragShaderName) :
        Shader(device)
    {
        SetShaderName(name);
        static shaderc::Compiler       compiler;
        static shaderc::CompileOptions options;

        auto& pathManager = PathManager::Get();

        std::filesystem::path shaderPath = pathManager.shaderPath;

        auto vertexCode = io::ReadFileToString(shaderPath / vetexShaderName);
        auto frageCode  = io::ReadFileToString(shaderPath / fragShaderName);

        auto vertexResult = compiler.CompileGlslToSpv(vertexCode, shaderc_glsl_vertex_shader, "vertex", options);

        if (vertexResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            throw std::runtime_error("Failed to compile vertex shader: " + std::string(vertexResult.GetErrorMessage()));
        }

        auto fragResult = compiler.CompileGlslToSpv(frageCode, shaderc_glsl_fragment_shader, "fragment", options);

        if (fragResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            throw std::runtime_error("Failed to compile fragment shader: " + std::string(fragResult.GetErrorMessage()));
        }

        auto vertexOutputBinary = std::vector<uint32_t>(vertexResult.begin(), vertexResult.end());
        auto fragOutputBinary   = std::vector<uint32_t>(fragResult.begin(), fragResult.end());

        m_vertexModule = device.vkDevice().createShaderModule(vk::ShaderModuleCreateInfo {
            .codeSize = vertexOutputBinary.size() * sizeof(uint32_t),
            .pCode    = reinterpret_cast<const uint32_t*>(vertexOutputBinary.data()),
        });

        m_fragModule = device.vkDevice().createShaderModule(vk::ShaderModuleCreateInfo {
            .codeSize = fragOutputBinary.size() * sizeof(uint32_t),
            .pCode    = reinterpret_cast<const uint32_t*>(fragOutputBinary.data()),
        });

        CollectMetaData(vertexOutputBinary, vk::ShaderStageFlagBits::eVertex);
        CollectMetaData(fragOutputBinary, vk::ShaderStageFlagBits::eFragment);
        GeneratePipelineLayout();
    }

    RasterShader::~RasterShader()
    {
        auto vkDevice = device.vkDevice();
        vkDevice.destroyShaderModule(m_vertexModule);
        vkDevice.destroyShaderModule(m_fragModule);
    }
} // namespace wind