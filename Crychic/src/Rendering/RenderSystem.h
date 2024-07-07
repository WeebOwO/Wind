#pragma once

#include "Core/SubSystem.h"
#include "Core/Window.h"

#include <Core/ConfigManager.h>
#include <ImGuiImplDiligent.hpp>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

#include "Scene/Scene.h"

using namespace Diligent;

namespace crychic
{
    // rendering engine class
    // convert the render graph to a job
    constexpr uint32_t kMaxFramesInFlight = 2;

    struct RenderConfig : Config
    {
        Window* window;
    };

    class RenderSystem : public SubSystem
    {
    public:
        explicit RenderSystem(Window* window);

        [[nodiscard]] ImGuiImplDiligent* GetImguiPtr() const { return m_imgui.get(); }

        void Init() override;
        void Quit() override;

        void Tick() override;

        void NewFrame();
        void EndFrame();

    private:
        RefCntAutoPtr<IRenderDevice>       m_device;
        RefCntAutoPtr<IDeviceContext>      m_context;
        RefCntAutoPtr<ISwapChain>          m_swapChain;
        std::unique_ptr<ImGuiImplDiligent> m_imgui;

        Scene* m_refScene;
    };

    extern RenderSystem* g_renderSystem;
} // namespace crychic
