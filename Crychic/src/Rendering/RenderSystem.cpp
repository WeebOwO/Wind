#include "RenderSystem.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <EngineFactoryD3D12.h>
#include <Win32NativeWindow.h>

namespace crychic
{
    RenderSystem* g_renderSystem = nullptr;

    void RenderSystem::Init()
    {
        // create renderer
        m_isInitialized = true;
    }

    void RenderSystem::Quit()
    {
        m_device    = nullptr;
        m_context   = nullptr;
        m_swapChain = nullptr;
    }

    RenderSystem::RenderSystem(Window* window)
    {
        Win32NativeWindow nativeWindow {glfwGetWin32Window(window->GetGLFWWindow())};
        auto*             factoryD3D12 = LoadGraphicsEngineD3D12();
        auto*             factory      = factoryD3D12();

        SwapChainDesc swapChainDesc {};

        EngineD3D12CreateInfo createInfo {};
        factory->CreateDeviceAndContextsD3D12(createInfo, &m_device, &m_context);
        factory->CreateSwapChainD3D12(
            m_device, m_context, swapChainDesc, FullScreenModeDesc {}, nativeWindow, &m_swapChain);
    }

    void RenderSystem::Tick()
    {
        // render
    }

} // namespace crychic