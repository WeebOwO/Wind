#pragma once

#include "std.h"

#include "VulkanHeader.h"

namespace wind
{
    class GPUDevice;

    struct RHIResource
    {
        RHIResource(GPUDevice& device);
        GPUDevice& device;
    };
} // namespace wind