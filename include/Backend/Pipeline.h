#pragma once

#include "Enum.h"

#include "Resource.h"
#include "Shader.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace wind
{
    class Pipeline : public Resource
    {
    public:
        Pipeline(Device* device, const vk::GraphicsPipelineCreateInfo& createInfo);
        Pipeline(Device* device, const vk::ComputePipelineCreateInfo& createInfo);

        virtual ~Pipeline();

    private:
        friend class Device;

        PipelineType         m_type;
        PassType             m_passType;
        vk::Pipeline         m_pipeline;
        vk::PipelineLayout   m_pipelineLayout;
        vk::Pipeline         m_pipelineCache;
        std::vector<Shader*> m_refShaders;
    };
} // namespace wind