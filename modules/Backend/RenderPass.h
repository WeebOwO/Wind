#pragma once

#include "Resource.h"

namespace wind
{
    class RenderPass : public Resource
    {
    public:
        RenderPass(Device* device, PassType type) : Resource(device, Tag::RenderPass) {}
        ~RenderPass() = default;

    private:
        vk::RenderPass m_renderPass;
    };
} // namespace wind