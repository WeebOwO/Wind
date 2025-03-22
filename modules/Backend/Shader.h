#pragma once

#include <unordered_map>

#include "Guard.h"
#include "Resource.h"

namespace wind
{
    enum class ShaderType : uint8_t
    {
        Unknown,
        Vertex,
        Fragment,
        Compute,
        RayGen,
        Miss,
        ClosestHit,
        AnyHit,
        Intersection,
        Count
    };

    struct BlobData
    {
        BlobData() = default;
        BlobData(ShaderType type) : type(type) {}

        ShaderType type = ShaderType::Unknown;

        std::vector<uint32_t>   data;   // SPIRV data
        vk::ShaderModule        module; // shader module
        vk::ShaderStageFlagBits stage;  // shader stage
        std::string             shaderCode;
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

    class Shader : public Resource
    {
    public:
        Shader(Device* device, const BlobData& blob);
        virtual ~Shader() = default;

        void InitRHI() override;
        void ReleaseRHI() override;

        auto GetBlobData() -> const BlobData& { return m_Blob; }
        vk::PipelineLayoutCreateInfo GetPipelineLayoutInfo() { return m_PipelineLayoutInfo; }

    private:
        friend class Device;

        void GenerateReflectionData();
        void ReflectShader(const BlobData& blob);
        void CreatePipelineLayoutInfo();

        BlobData                                        m_Blob;
        std::unordered_map<std::string, ShaderMetaData> m_ReflectionDatas;

        // pipeline layout
        vk::PipelineLayoutCreateInfo m_PipelineLayoutInfo;
    };
} // namespace wind