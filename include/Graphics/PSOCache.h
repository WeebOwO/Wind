#pragma once

#include <cstdint>

#include "Backend/Device.h"

namespace wind
{
    enum PipelineID : std::uint32_t
    {
        Forward = 0,
        ShadowMap,
        Count,
    };

    class PSOCache
    {
    public:
        PSOCache(Device* device);
        ~PSOCache();

        void Init();
        void Destroy();

    private:
        Device* m_device;
    };
} // namespace wind