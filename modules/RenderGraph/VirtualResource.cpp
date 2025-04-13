#include "VirtualResource.h"

namespace wind
{
    void VirtualResource::NeedByPass(PassNode* passNode)
    {
        if (first == nullptr)
        {
            first = passNode;
        }
        last = passNode;
        refCount++;
    }
} // namespace wind