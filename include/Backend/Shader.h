#pragma once

#include <unordered_map>

#include "Enum.h"
#include "Resource.h"

namespace wind
{
    struct BlobData
    {
    public:
        BlobData() = default;
        BlobData(const std::string& code, ShaderType type) : shaderCode(code), type(type) {}

        std::string shaderCode;
        ShaderType  type;

    private:
        friend class Shader;
        std::vector<uint32_t>   data;
        vk::ShaderModule        module;
        vk::ShaderStageFlagBits stage;
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
        Shader(Device* device, const BlobData& blob);
        virtual ~Shader();

        vk::ShaderStageFlagBits GetStage() const { return m_blob.stage; }
        vk::ShaderModule        GetModule() const { return m_blob.module; }

    private:
        friend class Device;

        void Init();

        void GenerateReflectionData();
        void ReflectShader(const BlobData& blob);

        ShaderType                                      m_type;
        BlobData                                        m_blob;
        std::unordered_map<std::string, ShaderMetaData> m_reflectionDatas;

        // store per material data
        vk::DescriptorSetLayout m_descriptorSetLayout;
        vk::DescriptorSet       m_descriptorSet;
    };
} // namespace wind