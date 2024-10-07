#include "Backend/Shader.h"

#include "Backend/Device.h"

namespace wind
{
    RasterShader::RasterShader(Device* device) : Resource(device, Tag::Shader) {}

    void RasterShader::Load(const std::string& path)
    {
        // Load shader from file
    }

    void RasterShader::PopShaderStageInfo(std::vector<vk::PipelineShaderStageCreateInfo>& stages)
    {
        for (auto& shader : m_shaders)
        {
            vk::PipelineShaderStageCreateInfo stageInfo;
            stageInfo.stage  = shader->GetStage();
            stageInfo.module = shader->GetModule();
            stageInfo.pName  = "main";
            stages.push_back(stageInfo);
        }
    }

    void RasterShader::PopPipelineLayoutInfo(vk::PipelineLayoutCreateInfo& layoutInfo)
    {
        // todo: generate pipeline layout info
        layoutInfo.setSetLayouts(m_descriptorSetLayouts);
    }

    RasterShader& RasterShader::operator<<(Shader* shader)
    {
        m_shaders.push_back(shader);
        return *this;
    }
} // namespace wind