#include "RenderGraph/RGAllocator.h"

namespace wind::rg
{
    RenderGraphAllocator::RenderGraphAllocator(Device* device) : m_device(device) {}
    RenderGraphAllocator::~RenderGraphAllocator() {}
} // namespace wind::rg