#include "Shader.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Core/Log.h"
#include "Device.h"

namespace wind
{
    static vk::ShaderStageFlagBits ConverToShaderStageFromType(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return vk::ShaderStageFlagBits::eVertex;
            case ShaderType::Fragment:
                return vk::ShaderStageFlagBits::eFragment;
            case ShaderType::Compute:
                return vk::ShaderStageFlagBits::eCompute;
            default:
                WIND_CORE_ERROR("Unsupported shader stage.");
                return vk::ShaderStageFlagBits::eAll;
        }
    };

    Shader::Shader(Device* device, const BlobData& blob) : Resource(device), m_Blob(blob)
    {
        m_Blob.stage = ConverToShaderStageFromType(blob.type);
    }

    void Shader::ReflectShader(const BlobData& blob)
    {
        // Reflect shader
        spirv_cross::CompilerGLSL    compiler(std::move(blob.data));
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        auto collectResource = [&](auto resource, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderFlags) {
            if (m_ReflectionDatas.find(resource.name) == m_ReflectionDatas.end())
            {
                std::string_view             resourceName = resource.name;
                uint32_t                     set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
                uint32_t                     binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                const spirv_cross::SPIRType& type    = compiler.get_type(resource.type_id);
                uint32_t                     typeArraySize = type.array.size();
                uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
                ShaderMetaData               metaData {set, binding, count, descriptorType, shaderFlags};
                m_ReflectionDatas[resource.name] = metaData;
            }
            else
            {
                m_ReflectionDatas[resource.name].shaderStageFlag |= shaderFlags;
            }
        };

        // Reflect input
        for (auto& resource : resources.uniform_buffers)
        {
            collectResource(resource, vk::DescriptorType::eUniformBuffer, blob.stage);
        }
    }

    void Shader::GenerateReflectionData()
    {
        // Reflect shader
        ReflectShader(m_Blob);
    }

    void Shader::InitRHI()
    {
        // Create shader module
        vk::Device vkDevice = m_Device->GetDevice();

        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;

        {
            // Create shader module
            shaderc::SpvCompilationResult result;

            switch (m_Blob.stage)
            {
                case vk::ShaderStageFlagBits::eVertex:
                    result =
                        compiler.CompileGlslToSpv(m_Blob.shaderCode, shaderc_glsl_vertex_shader, "vertex", options);
                    break;
                case vk::ShaderStageFlagBits::eFragment:
                    result =
                        compiler.CompileGlslToSpv(m_Blob.shaderCode, shaderc_glsl_fragment_shader, "fragment", options);
                    break;
                default:
                    WIND_CORE_ERROR("Unsupported shader stage.");
                    return; // Handle error or throw an exception
            }

            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                WIND_CORE_ERROR("Failed to compile shader: {0}", result.GetErrorMessage());
                return; // Handle error or throw an exception
            }

            uint32_t codeSize = result.cend() - result.cbegin();
            m_Blob.data.assign(result.cbegin(), result.cend());

            vk::ShaderModuleCreateInfo createInfo;
            createInfo.setCode(m_Blob.data);

            m_Blob.module = vkDevice.createShaderModule(createInfo);
        }

        GenerateReflectionData();
    }

    void Shader::ReleaseRHI()
    {
        vk::Device vkDevice = m_Device->GetDevice();
        vkDevice.destroyShaderModule(m_Blob.module);
    }
} // namespace wind