#pragma once

#include <unordered_map>

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    struct BlobData
    {
        BlobData() = default;

        vk::ShaderStageFlagBits stage;
        std::vector<uint32_t>   spirv; // SPIRV data
        vk::ShaderModule        module;
    };

    struct ShaderMetaData
    {
        uint32_t             set;
        uint32_t             binding;
        uint32_t             count;
        vk::DescriptorType   descriptorType;
        vk::ShaderStageFlags shaderStageFlag;
    };

    struct PushConstantMetaData
    {
        uint32_t             size;
        uint32_t             offset;
        vk::ShaderStageFlags shadeshaderStageFlag;
    };

    // abstraction about vulkan shader module
    class Shader : public Resource
    {
    public:
        Shader(Device* device, const BlobData& blob);
        virtual ~Shader() = default;

        void InitRHI() override;
        void ReleaseRHI() override;

        auto GetBlobData() -> const BlobData& { return m_Blob; }

        vk::ShaderModule GetNativeHandle() const { return m_Blob.module; }

    private:
        friend class Device;

        void GenerateReflectionData();
        void ReflectShader(const BlobData& blob);

        BlobData                                        m_Blob;
        std::unordered_map<std::string, ShaderMetaData> m_ReflectionDatas;
    };
} // namespace wind