#include "Utils.h"

#include "Device.h"
#include "Engine/RuntimeContext.h"

namespace wind::utils
{
    vk::Sampler CreateSampler(const vk::SamplerCreateInfo& createInfo)
    {
        assert(g_runtimeContext.device);
        vk::Device device = g_runtimeContext.device->vkDevice();
        return device.createSampler(createInfo);
    }

    vk::Sampler CreateDefaultSampler()
    {
        auto samplerCreateInfo = vk::SamplerCreateInfo {.magFilter     = vk::Filter::eLinear,
                                                        .minFilter     = vk::Filter::eLinear,
                                                        .mipmapMode    = vk::SamplerMipmapMode::eLinear,
                                                        .addressModeU  = vk::SamplerAddressMode::eClampToEdge,
                                                        .addressModeV  = vk::SamplerAddressMode::eClampToEdge,
                                                        .addressModeW  = vk::SamplerAddressMode::eClampToEdge,
                                                        .mipLodBias    = 0.0f,
                                                        .maxAnisotropy = 1.0f,
                                                        .minLod        = 0.0f,
                                                        .maxLod        = 100.0f,
                                                        .borderColor   = vk::BorderColor::eFloatOpaqueWhite};

        return CreateSampler(samplerCreateInfo);
    }
} // namespace wind::utils