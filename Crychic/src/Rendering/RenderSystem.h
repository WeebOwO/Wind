#pragma once

#include "Core/SubSystem.h"
#include "Core/Window.h"

#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

using namespace Diligent;

namespace crychic
{
    // rendering engine class
    // convert the render graph to a job
    constexpr uint32_t kMaxFramesInFlight = 2;

    class RenderSystem : public SubSystem
    {
    public:
        explicit RenderSystem(Window* window);

        void Init() override;
        void Quit() override;

        void Tick();

    private:
        RefCntAutoPtr<IRenderDevice>  m_device;
        RefCntAutoPtr<IDeviceContext> m_context;
        RefCntAutoPtr<ISwapChain>     m_swapChain;
    };

    extern RenderSystem* g_renderSystem;
} // namespace crychic
