#pragma once

#include <unordered_map>

#include "Core/Vector.h"
#include "Enum.h"
#include "Resource.h"

namespace wind
{
    struct BlobData
    {
        vk::ShaderStageFlagBits stage;
        vk::ShaderModule        module;
        std::vector<uint32_t>   data;
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

    class Shader : public Resource
    {
    public:
        Shader(Device* device, ShaderType type);
        virtual ~Shader();

        void AddBlob(const BlobData& blob);

    protected:
        void GenerateReflectionData();
        void ReflectShader(const BlobData& blob);

        ShaderType                                      m_type;
        wind::vector<BlobData>                          m_blobs;
        wind::vector<vk::DescriptorSetLayout>           m_descriptorSetLayouts;
        wind::vector<vk::DescriptorSet>                 m_descriptorSets;
        std::unordered_map<std::string, ShaderMetaData> m_reflectionDatas;
    };

    class RasterShader final : public Shader
    {
    public:
        RasterShader(Device* device);
        ~RasterShader() override = default;

        void Load(const std::string& path);

        vk::ShaderModule GetVertexShaderModule() const;
        vk::ShaderModule GetFragmentShaderModule() const;

        void PopShaderStageInfo(std::vector<vk::PipelineShaderStageCreateInfo>& stages);
        void PopPipelineLayoutInfo(vk::PipelineLayoutCreateInfo& layoutInfo);

        void Init() override;
        void Destroy() override;
    };
} // namespace wind