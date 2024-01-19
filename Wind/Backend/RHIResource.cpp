#include "RHIResource.h"

#include "Device.h"

namespace wind
{
    RHIResource::RHIResource(GPUDevice& device) : device(device) {}
} // namespace wind