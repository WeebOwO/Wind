#include "RenderSystem.h"

#include <EngineFactoryD3D12.h>
#include <ImGuiImplWin32.hpp>
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
        HWND hwnd = window->GetNativeWindow();
        Win32NativeWindow nativeWindow{hwnd};

        auto* factoryD3D12 = LoadGraphicsEngineD3D12();
        auto* factory      = factoryD3D12();

        SwapChainDesc swapChainDesc {};

        EngineD3D12CreateInfo createInfo {};
        factory->CreateDeviceAndContextsD3D12(createInfo, &m_device, &m_context);
        factory->CreateSwapChainD3D12(
            m_device, m_context, swapChainDesc, FullScreenModeDesc {}, nativeWindow, &m_swapChain);

        ImGuiDiligentCreateInfo imguiCreateInfo {m_device, m_swapChain->GetDesc()};

        m_imgui = ImGuiImplWin32::Create(imguiCreateInfo, hwnd);
    }

    void RenderSystem::NewFrame()
    {
        const auto& swapChainDesc = m_swapChain->GetDesc();
        m_imgui->NewFrame(swapChainDesc.Width, swapChainDesc.Height, swapChainDesc.PreTransform);
    }

    void RenderSystem::EndFrame() { m_imgui->EndFrame(); }

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

        // ui pass
        m_imgui->Render(m_context);
        m_swapChain->Present();
    }

} // namespace crychic