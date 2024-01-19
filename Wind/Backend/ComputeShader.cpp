#include "ComputeShader.h"

#include "Device.h"

#include "Core/Log.h"

namespace wind
{
    ComputeShader::ComputeShader(GPUDevice&                   device,
                                 const std::string&           debugName,
                                 const std::vector<uint32_t>& spirvCode) :
        Shader(device)
    {
        SetShaderName(debugName);
        auto vkDevice = device.GetVkDeviceHandle();

        // create Pipeline cache
        bool                        cacheValid = spirvCode.size() != 0;
        vk::PipelineCacheCreateInfo cacheCreateInfo {
            .flags           = {},
            .initialDataSize = cacheValid ? sizeof(uint32_t) * spirvCode.size() : 0,
            .pInitialData    = cacheValid ? spirvCode.data() : nullptr,
        };

        m_cache = vkDevice.createPipelineCache(cacheCreateInfo);

        vk::ShaderModuleCreateInfo shaderCreateInfo {.codeSize = sizeof(uint32_t) * spirvCode.size(),
                                                     .pCode    = spirvCode.data()};
        m_computeModule = vkDevice.createShaderModule(shaderCreateInfo);

        CollectMetaData(spirvCode, vk::ShaderStageFlagBits::eCompute);
        GeneratePipelineLayout();

        vk::PipelineShaderStageCreateInfo shaderStage {
            .stage  = vk::ShaderStageFlagBits::eCompute,
            .module = m_computeModule,
            .pName  = "main",
        };

        vk::ComputePipelineCreateInfo pipelineCreateInfo {.stage = shaderStage, .layout = m_layout};

        m_pipeline = vkDevice.createComputePipeline(m_cache, pipelineCreateInfo).value;
    }

    void ComputeShader::BindCommandBuffer(const vk::CommandBuffer& cmdBuffer) const
    {
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);
    }

    ComputeShader::~ComputeShader()
    {
        auto vkDevice = device.GetVkDeviceHandle();
        vkDevice.waitIdle();
        vkDevice.destroyPipelineCache(m_cache);
        vkDevice.destroyShaderModule(m_computeModule);
        vkDevice.destroyPipeline(m_pipeline);
    }
} // namespace wind