#pragma once

#include "Backend/Pipeline.h"
#include <vector>

#include "Command.h"
#include "Core/NonCopy.h"
#include "Resource.h"

namespace wind
{
    class Device;

    class CommandStream final : public Resource
    {
    public:
        NON_COPYABLE(CommandStream);

        CommandStream(Device* device, RenderCommandQueueType queueType);
        ~CommandStream() noexcept;

        vk::CommandBuffer GetCommandBuffer() { return m_commandBuffer; }

        void BeginRendering(const vk::RenderingInfo& info);
        void EndRendering();

        void BindPipeline(const Pipeline& pipeline);
        
    private:
        void Init();
        void Destroy();

        vk::CommandBuffer      m_commandBuffer;
        vk::CommandPool        m_commandPool;
        RenderCommandQueueType m_queueType;
    };
} // namespace wind