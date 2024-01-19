#pragma once

#include "std.h"

#include "RHIResource.h"
#include "VulkanHeader.h"

namespace wind
{
    class Shader : public RHIResource
    {
    public:
        enum class ShaderTag : uint8_t
        {
            ComputeShader = 0,
            RasterShader,
        };
        Shader(GPUDevice& device);
        ~Shader();

        virtual void BindCommandBuffer(const vk::CommandBuffer& cmdBuffer) const;

        auto GetShaderName() const { return m_debugName; }
        auto GetPipelineLayout() const { return m_layout; }
        auto SetShaderName(const std::string& name) { m_debugName = name; }
        void CollectMetaData(const std::vector<uint32_t>& spirvCode, vk::ShaderStageFlags flag);
        void GeneratePipelineLayout();

        struct ShaderBinding
        {
            uint32_t             set;
            uint32_t             binding;
            uint32_t             count;
            vk::DescriptorType   descriptorType;
            vk::ShaderStageFlags shaderStageFlag;
        };

        struct PushConstantBinding
        {
            uint32_t             size;
            uint32_t             offset;
            vk::ShaderStageFlags shadeshaderStageFlag;
        };

        std::string                                    m_debugName;
        vk::PipelineLayout                             m_layout;
        std::unordered_map<std::string, ShaderBinding> m_bindings;

        std::vector<vk::PushConstantRange>                   m_pushRanges;
        std::unordered_map<std::string, PushConstantBinding> m_pushConstantBinding;

        std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    };
} // namespace wind