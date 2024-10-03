#pragma once

#include "Backend/Device.h"

namespace wind
{
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