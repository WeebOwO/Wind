#include "Shader.h"

#include <spirv_cross/spirv_glsl.hpp>

#include "Device.h"

#include "Core/Log.h"

namespace wind
{
    Shader::Shader(GPUDevice& device) : RHIResource(device) {}

    void Shader::CollectMetaData(const std::vector<uint32_t>& spirvCode, vk::ShaderStageFlags flag)
    {
        spirv_cross::CompilerGLSL    compiler(spirvCode);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        auto collectResource = [&](auto resource, vk::DescriptorType descriptorType) {
            if (m_bindings.find(resource.name) == m_bindings.end())
            {
                std::string_view             resourceName = resource.name;
                uint32_t                     set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
                uint32_t                     binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                const spirv_cross::SPIRType& type    = compiler.get_type(resource.type_id);
                uint32_t                     typeArraySize = type.array.size();
                uint32_t                     count         = typeArraySize == 0 ? 1 : type.array[0];
                ShaderBinding                metaData {set, binding, count, descriptorType, flag};
                m_bindings[resource.name] = metaData;
            }
            else
            {
                m_bindings[resource.name].shaderStageFlag |= flag;
            }
        };

        for (auto& resource : resources.uniform_buffers)
        {
            collectResource(resource, vk::DescriptorType::eUniformBuffer);
        }

        for (auto& resource : resources.sampled_images)
        {
            collectResource(resource, vk::DescriptorType::eCombinedImageSampler);
        }

        for (auto& resource : resources.separate_samplers)
        {
            collectResource(resource, vk::DescriptorType::eSampler);
        }

        for (auto& resource : resources.separate_images)
        {
            collectResource(resource, vk::DescriptorType::eSampledImage);
        }

        for (auto& resource : resources.storage_buffers)
        {
            collectResource(resource, vk::DescriptorType::eStorageBuffer);
        }

        for (const auto& resource : resources.push_constant_buffers)
        {
            std::string                  resourceName = resource.name;
            const spirv_cross::SPIRType& type         = compiler.get_type(resource.type_id);
            uint32_t                     size         = compiler.get_declared_struct_size(type);
            if (!m_pushConstantBinding.contains(resourceName))
            {
                m_pushConstantBinding[resourceName] =
                    PushConstantBinding {.size = size, .offset = 0, .shadeshaderStageFlag = flag};
            }
            else
            {
                m_pushConstantBinding[resourceName].shadeshaderStageFlag |= flag;
            }
        }
    }

    void Shader::GeneratePipelineLayout()
    {
        auto vkDevice = device.vkDevice();

        std::vector<vk::DescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos;
        std::vector<vk::DescriptorSetLayoutBinding>    layoutBindings;

        std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> m_setGroups;

        for (const auto& [resourceName, metaData] : m_bindings)
        {
            vk::DescriptorSetLayoutBinding binding;
            vk::ShaderStageFlags           stageFlags = metaData.shaderStageFlag;
            uint32_t                       set        = metaData.set;

            binding.setBinding(metaData.binding)
                .setDescriptorCount(metaData.count)
                .setDescriptorType(metaData.descriptorType)
                .setStageFlags(stageFlags);

            layoutBindings.push_back(binding);
            m_setGroups[set].push_back(binding);
        }

        for (const auto& [setIndex, bindingVecs] : m_setGroups)
        {
            vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
            descriptorSetLayoutCreateInfo.setBindingCount(bindingVecs.size()).setBindings(bindingVecs);
            vk::DescriptorSetLayout setLayout = vkDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

            m_descriptorSetLayouts.push_back(setLayout);
        }

        for (const auto& [resourceName, pushBinding] : m_pushConstantBinding)
        {
            vk::PushConstantRange range {
                .stageFlags = pushBinding.shadeshaderStageFlag, .offset = pushBinding.offset, .size = pushBinding.size};
            m_pushRanges.push_back(range);
        }

        vk::PipelineLayoutCreateInfo layoutCreateInfo {.setLayoutCount = (uint32_t)m_descriptorSetLayouts.size(),
                                                       .pSetLayouts    = m_descriptorSetLayouts.data(),
                                                       .pushConstantRangeCount = (uint32_t)m_pushRanges.size(),
                                                       .pPushConstantRanges    = m_pushRanges.data()};

        m_layout = vkDevice.createPipelineLayout(layoutCreateInfo);
    }

    Shader::~Shader()
    {
        auto vkDevice = device.vkDevice();
        vkDevice.waitIdle();

        vkDevice.destroyPipelineLayout(m_layout);

        for (auto& setlayout : m_descriptorSetLayouts)
        {
            vkDevice.destroyDescriptorSetLayout(setlayout);
        }
    }

    void Shader::BindCommandBuffer(const vk::CommandBuffer& cmdBuffer) const {}
} // namespace wind