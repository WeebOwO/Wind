#include "RasterShader.h"

#include "Backend/Device.h"

#include "Engine/RuntimeContext.h"
#include "Resource/Loader.h"

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


    RasterShader::~RasterShader()
    {
        auto vkDevice = device.GetVkDeviceHandle();
        vkDevice.destroyShaderModule(m_vertexModule);
        vkDevice.destroyShaderModule(m_fragModule);
    }
} // namespace wind