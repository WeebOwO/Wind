#include "RenderGraphTexture.h"

namespace wind::rg 
{
    void RenderGraphTexture::Create(rg::RenderGraphAllocator* allocator, const std::string& name, const Descriptor& descriptor)
    {
        this->desc = descriptor;
        this->name = name;
    }
}