#pragma once

namespace wind
{
    enum class CullingMethod
    {
        None = 0,
        Normal,  // Do traditional cpu culling
        Compute, // using Compute shader culling
    };

    class CullingManager
    {   
        
    };
} // namespace wind