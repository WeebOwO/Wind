#include "RenderSystem.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <EngineFactoryD3D12.h>
#include <Win32NativeWindow.h>

#include "PSOCache.h"
#include "ShaderCache.h"

namespace crychic
{
    RenderSystem* g_renderSystem = nullptr;

    void RenderSystem::Init()
    {
        // create renderer
        ShaderCache::Init(m_device);
        PSOCache::Init(m_device);
        m_isInitialized = true;
        g_renderSystem  = this;
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

        m_swapChain->Resize(window->GetWidth(), window->GetHeight());
    }

    void RenderSystem::Tick()
    {
        // render
        auto*       pRTV         = m_swapChain->GetCurrentBackBufferRTV();
        auto*       pDSV         = m_swapChain->GetDepthBufferDSV();
        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};

        m_context->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        m_context->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_context->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        m_context->SetPipelineState(PSOCache::GetPSO(PsoStateID::Triangle));

        DrawAttribs drawAttribs;
        drawAttribs.NumVertices = 3;
        m_context->Draw(drawAttribs);

        m_swapChain->Present();
    }

} // namespace crychic