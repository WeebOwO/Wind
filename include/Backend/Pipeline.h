#pragma once

#include "Enum.h"

#include "Resource.h"
#include "Shader.h"

namespace wind
{
    class Pipeline : public Resource
    {
    public:
        Pipeline(Device* device, PipelineType type, std::vector<Shader*> shaders) :
            Resource(device, Tag::Pipeline), m_type(type), m_refShaders(shaders)
        {}
        virtual ~Pipeline();

    private:
        friend class Device;

        PipelineType         m_type;
        vk::Pipeline         m_pipeline;
        vk::PipelineLayout   m_pipelineLayout;
        vk::Pipeline         m_pipelineCache;
        std::vector<Shader*> m_refShaders;
    };
} // namespace wind