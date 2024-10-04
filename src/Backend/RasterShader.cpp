#include "Backend/Shader.h"

#include "Backend/Device.h"

namespace wind
{
    RasterShader::RasterShader(Device* device) : Resource(device, Tag::Shader) {}

    void RasterShader::Load(const std::string& path)
    {
        // Load shader from file
    }

    void RasterShader::PopShaderStageInfo(std::vector<vk::PipelineShaderStageCreateInfo>& stages) {}

    void RasterShader::PopPipelineLayoutInfo(vk::PipelineLayoutCreateInfo& layoutInfo)
    {
        // todo: generate pipeline layout info
    }
} // namespace wind