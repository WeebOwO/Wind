#pragma once

#include "SwapChain.h"

#include "Context.h"

namespace wind
{
    SwapChain::SwapChain(Context* context, const SwapChainDesc& desc) :
        Resource(context, ResourceType::SwapChain), m_desc(desc)
    {}

    void SwapChain::Resize(uint32_t width, uint32_t height)
    {
        // resize swap chain
        m_desc.width  = width;
        m_desc.height = height;
    }

    SwapChain::~SwapChain()
    {
        // destroy swap chain
        auto device = m_context->GetDevice();
    }
} // namespace wind