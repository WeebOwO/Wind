#include "Backend/Enum.h"

#include <shaderc/shaderc.hpp>

#include "Backend/Device.h"
#include "Backend/Shader.h"
#include "Core/Log.h"

namespace wind
{
    RasterShader::RasterShader(Device* device) : Shader(device, ShaderType::Raster) {}

    void RasterShader::Load(const std::string& path)
    {
        // Load shader from file
    }

    void RasterShader::Init()
    {
        // Create shader module
        vk::Device vkDevice = m_device->GetDevice();

        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;

        for (auto blob : m_blobs)
        {
            // Create shader module
            if (blob.stage == vk::ShaderStageFlagBits::eVertex)
            {
                shaderc::SpvCompilationResult result =
                    compiler.CompileGlslToSpv(blob.shaderCode, shaderc_glsl_vertex_shader, "vertex", options);
                if (result.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    WIND_CORE_ERROR("Failed to compile vertex shader: {0}", result.GetErrorMessage());
                }

                vk::ShaderModuleCreateInfo createInfo;
                createInfo.codeSize = result.cend() - result.cbegin();
                createInfo.pCode    = result.cbegin();

                blob.module = vkDevice.createShaderModule(createInfo);
            }
            else if (blob.stage == vk::ShaderStageFlagBits::eFragment)
            {
                shaderc::SpvCompilationResult result =
                    compiler.CompileGlslToSpv(blob.shaderCode, shaderc_glsl_fragment_shader, "fragment", options);
                if (result.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    WIND_CORE_ERROR("Failed to compile fragment shader: {0}", result.GetErrorMessage());
                }

                vk::ShaderModuleCreateInfo createInfo;
                createInfo.codeSize = result.cend() - result.cbegin();
                createInfo.pCode    = result.cbegin();

                blob.module = vkDevice.createShaderModule(createInfo);
            }
        }

        GenerateReflectionData();
    }

    void RasterShader::Destroy() {}

    void RasterShader::PopShaderStageInfo(std::vector<vk::PipelineShaderStageCreateInfo>& stages)
    {
        for (auto blob : m_blobs)
        {
            vk::PipelineShaderStageCreateInfo stageInfo;
            stageInfo.stage  = blob.stage;
            stageInfo.module = blob.module;
            stageInfo.pName  = "main";

            stages.push_back(stageInfo);
        }
    }

    void RasterShader::PopPipelineLayoutInfo(vk::PipelineLayoutCreateInfo& layoutInfo)
    {
        // todo: generate pipeline layout info
        layoutInfo.setLayoutCount = m_descriptorSetLayouts.size();
        layoutInfo.pSetLayouts    = m_descriptorSetLayouts.data();
    }
} // namespace wind