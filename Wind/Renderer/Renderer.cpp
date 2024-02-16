#include "Renderer.h"

#include "Backend/ImGuiContext.h"
#include "Backend/PipelineBuilder.h"

#include "Resource/Loader.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "Material.h"
#include "PsoCache.h"

#include "Backend/Command.h"
#include "Backend/Device.h"
#include "Backend/RasterShader.h"
#include "Backend/SwapChain.h"

#include "RenderGraph/RenderGraphID.h"
#include "RenderGraph/RenderGraphTexture.h"
#include "RenderGraph/ResourceNode.h"

#include "Core/Log.h"

namespace wind
{
    void FrameParms::Init(vk::Device device)
    {
        vk::FenceCreateInfo fenceCreateInfo {.flags = vk::FenceCreateFlagBits::eSignaled};
        flightFence = device.createFence(fenceCreateInfo);

        imageAvailableSemaphore = device.createSemaphore({});
        renderFinishedSemaphore = device.createSemaphore({});

        dynamicDescriptorAllocator = ref::Create<DescriptorAllocator>(device);
    }

    void FrameParms::Destroy(vk::Device device)
    {
        device.destroyFence(flightFence);
        device.destroySemaphore(imageAvailableSemaphore);
        device.destroySemaphore(renderFinishedSemaphore);
    }

    Renderer::Renderer(const Window& window, const RenderConfig& config) : m_renderConfig(config)
    {
        auto& device = GPUDevice::Get();
        for (auto& data : m_frameParams)
        {
            data.Init(device);
        }

        m_swapchain   = scope::Create<Swapchain>(device, window);
        m_uiContext   = scope::Create<WindUIContext>(device, window, *m_swapchain);
        m_renderGraph = scope::Create<RenderGraph>();
        m_shaderMap   = scope::Create<ShaderMap>();

        auto basePassShader  = device.CreateRastShader("BasePassShader", "Triangle.vert", "Triangle.frag");
        auto compositeShader = device.CreateRastShader("CompositeShader", "FullScreen.vert", "Composite.frag");

        m_shaderMap->CacheRasterShader(basePassShader);
        m_shaderMap->CacheRasterShader(compositeShader);

        // init material manager
        m_materialManager = scope::Create<MaterialManager>();
        m_materialManager->InitDefaultMaterial(*m_shaderMap);

        m_psoCache = scope::Create<PsoCache>(device, *m_shaderMap);

        m_commandManager = scope::Create<CommandBufferManager>(device, 1, m_renderConfig.commandBufferPerThread);
    }

    Renderer::~Renderer()
    {
        for (auto& data : m_frameParams)
        {
            data.Destroy(GPUDevice::Get());
        }
        m_psoCache->Destroy();
    }

    void Renderer::Init(const Window& window, const RenderConfig& config) { s_instance = new Renderer(window, config); }

    void Renderer::Quit() { delete s_instance; }

    void Renderer::GeneratePSO(const std::string& assetPath) { PipelineBuilder builder; }

    GPUTexture* Renderer::GetRenderGraphOutput()
    {
        auto& blackBoard = m_renderGraph->GetBlackBoard();
        auto  output     = blackBoard.Get<RenderGraphTexture>("output");
        return m_renderGraph->Get(output)->GetTexture();
    }

    RenderGraph& Renderer::BeginFrame()
    {
        auto& frameData = GetCurrentFrameData();

        frameData.swapchainImageIndex =
            m_swapchain->AcquireNextImage(frameData.flightFence, frameData.imageAvailableSemaphore).value();
        m_commandManager->ResetPool(m_frameNumber);
        frameData.cmdBuffer = m_commandManager->GetCommandBuffer(m_frameNumber, 0, true);

        m_renderGraph->SetupSwapChain(*m_swapchain);
        m_renderGraph->SetupFrameData(frameData);

        return *m_renderGraph;
    }

    void Renderer::NextFrame()
    {
        m_renderGraph->Exec();
        m_frameNumber = (m_frameNumber + 1) % RenderConfig::MAX_FRAME_IN_FLIGHT;
    }

} // namespace wind