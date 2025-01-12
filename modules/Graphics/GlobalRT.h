#pragma once

#include <cstdint>

#include "Backend/Image.h"

namespace wind
{
    enum RTID : uint8_t
    {
        SceneColor = 0,
        SceneDepth,
        Count,
    };

    class RTManager
    {
    public:
        RTManager();
        ~RTManager();

        void Init();
        void CreateRT(RTID id, uint32_t width, uint32_t height);

    private:
        Image m_images[RTID::Count];
    };
} // namespace wind