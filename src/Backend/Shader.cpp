#include "Backend/Shader.h"

#include <spirv_cross/spirv_glsl.hpp>

#include "Backend/Device.h"

namespace wind
{
    Shader::Shader(Device* device, ShaderType type) : Resource(device, Tag::Shader), m_type(type) {}

    Shader::~Shader()
    {
        vk::Device vkDevice = m_device->GetDevice();
        for (auto& blob : m_blobs)
        {
            vkDevice.destroyShaderModule(blob.module);
        }

        // Destroy descriptor set layout
        for (auto& descriptorSetLayout : m_descriptorSetLayouts)
        {
            vkDevice.destroyDescriptorSetLayout(descriptorSetLayout);
        }
    }

    void Shader::AddBlob(const BlobData& blob) { m_blobs.push_back(blob); }

    void Shader::ReflectShader(const BlobData& blob)
    {
        // Reflect shader
        spirv_cross::CompilerGLSL    compiler(std::move(blob.data));
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        auto collectResource = [&](auto resource, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderFlags) {
            if (m_reflectionDatas.find(resource.name) == m_reflectionDatas.end())
            {
                std::string_view             resourceName = resource.name;
                uint32_t                     set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
                uint32_t                     binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                const spirv_cross::SPIRType& type    = compiler.get_type(resource.type_id);
                uint32_t                     typeArraySize = type.array.size();
                uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
                ShaderMetaData               metaData {set, binding, count, descriptorType, shaderFlags};
                m_reflectionDatas[resource.name] = metaData;
            }
            else
            {
                m_reflectionDatas[resource.name].shaderStageFlag |= shaderFlags;
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
        // Collect resource
        // Generate reflection data
        for (auto blob : m_blobs)
        {
            // Reflect shader
            ReflectShader(blob);
        }
    }
} // namespace wind