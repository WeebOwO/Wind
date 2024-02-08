#include "Renderer.h"

#include "Backend/PipelineBuilder.h"

#include "Engine/RuntimeContext.h"
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

    void Renderer::Init()
    {
        for (auto& data : m_frameParams)
        {
            data.Init(m_device);
        }

        m_renderGraph = scope::Create<RenderGraph>();

        m_shaderMap = scope::Create<ShaderMap>();

        std::filesystem::path shaderPath = g_runtimeContext.pathManager.shaderPath;

        auto basePassShader  = m_device.CreateRastShader("BasePassShader", "Triangle.vert", "Triangle.frag");
        auto compositeShader = m_device.CreateRastShader("CompositeShader", "FullScreen.vert", "Composite.frag");

        m_shaderMap->CacheRasterShader(basePassShader);
        m_shaderMap->CacheRasterShader(compositeShader);

        // init material manager
        m_materialManager = scope::Create<MaterialManager>();
        m_materialManager->InitDefaultMaterial(*m_shaderMap);

        m_psoCache = scope::Create<PsoCache>(m_device, *m_shaderMap);

        m_commandManager = scope::Create<CommandBufferManager>(m_device, 1, m_renderConfig.commandBufferPerThread);
    }

    void Renderer::Quit()
    {
        for (auto& data : m_frameParams)
        {
            data.Destroy(m_device);
        }
        m_psoCache->Destroy();
    }

    void Renderer::GeneratePSO(const std::string& assetPath)
    {
        PipelineBuilder builder;
        // generate gbuffer pso
    }

    GPUTexture* Renderer::GetRenderGraphOutput()
    {
        auto& blackBoard = m_renderGraph->GetBlackBoard();
        auto  output     = blackBoard.Get<RenderGraphTexture>("output");
        return m_renderGraph->Get(output)->GetTexture();
    }

    RenderGraph& Renderer::BeginFrame(const Swapchain& swapchain)
    {
        auto& frameData = GetCurrentFrameData();

        frameData.swapchainImageIndex =
            swapchain.AcquireNextImage(frameData.flightFence, frameData.imageAvailableSemaphore).value();
        m_commandManager->ResetPool(m_frameNumber);
        frameData.cmdBuffer = m_commandManager->GetCommandBuffer(m_frameNumber, 0, true);

        m_renderGraph->SetupSwapChain(swapchain);
        m_renderGraph->SetupFrameData(frameData);

        return *m_renderGraph;
    }

    void Renderer::NextFrame()
    {
        m_renderGraph->Exec();
        m_frameNumber = (m_frameNumber + 1) % RenderConfig::MAX_FRAME_IN_FLIGHT;
    }
} // namespace wind