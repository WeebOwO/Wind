#include "Shader.h"

#include <spirv_cross/spirv_glsl.hpp>

#include "Core/Log.h"
#include "Device.h"

namespace wind
{
    Shader::Shader(Device* device, const BlobData& blob) : Resource(device), m_Blob(blob)
    {}

    void Shader::ReflectShader(const BlobData& blob)
    {
        // Reflect shader
        spirv_cross::CompilerGLSL    compiler(std::move(blob.spirv));
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

        vk::ShaderModuleCreateInfo createInfo 
        {
            .codeSize = m_Blob.spirv.size() * sizeof(uint32_t),
            .pCode = m_Blob.spirv.data(),
        };

        m_Blob.module = vkDevice.createShaderModule(createInfo);

        // GenerateReflectionData();
    }

    void Shader::ReleaseRHI()
    {
        vk::Device vkDevice = m_Device->GetDevice();
        vkDevice.destroyShaderModule(m_Blob.module);
    }
} // namespace wind