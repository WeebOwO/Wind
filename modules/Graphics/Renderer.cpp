#include "Renderer.h"

#include "Backend/Utils.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"
#include "RenderGraph/RenderGraph.h"

namespace wind
{
    Renderer::Renderer() : m_FrameCounter(0)
    {
        global::Init();
        // initialize the renderer
        m_Window = std::make_unique<Window>("Wind", 800, 600);
        m_Window->Init();

        DeviceExtensions extensions = {.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, .enableValidationLayers = true};
        m_Device                    = Device::Create(extensions, m_Window.get());
        m_Swapchain                 = m_Device->CreateResourceUnique<Swapchain>(800, 600);
        m_ShaderLibrary             = std::make_unique<ShaderLibrary>();
        m_PipelineCache             = std::make_unique<PSOCache>();

        // initialize the persistent memory with 1MB
        m_LinearAllocator = new LinearAllocator(1024 * 1024);

        WIND_CORE_INFO("Renderer initialized");
        // create frame data
        CreateFrameData();
        RegisterDeletionQueue();
    }

    Renderer::~Renderer()
    {
        WIND_CORE_INFO("Renderer shutting down");
        delete m_LinearAllocator;
        m_Device->WaitIdle();
        m_MainDelelteQueue.Flush();
        global::Shutdown();
    }

    void Renderer::Run()
    {
        m_ShaderLibrary->Init(m_Device.get());
        m_PipelineCache->Init(m_Device.get(), m_ShaderLibrary.get());
        // initialize the render graph
        InitRenderGraph();

        while (!m_Window->ShouldClose())
        {
            m_Window->Update();
            m_ShaderLibrary->Update();
            ProcessDirtyShaders();
            RecordRenderGraph();
        }
    }

    void Renderer::ParseCommandLine(const CommandLineArguments& args)
    {
        // parse command line arguments
    }

    void Renderer::RecordRenderGraph()
    {
        // draw a triangle
        BeginFrame();

        auto& frame = GetCurrentFrameData();

        RenderGraphUpdateContext context = {m_FrameCounter, frame.commandStream.get()};
        m_RenderGraph->PrepareFrame(context);

        // build the render graph
        for (auto& pass : m_ActivePassRoot)
        {
            pass->RecordRenderGrpah(*m_RenderGraph);
        }

        vk::CommandBuffer cmdBuffer = frame.commandStream->Begin();

        // begin render pass
        vk::ClearValue clearValue = {};
        clearValue.color          = vk::ClearColorValue(std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f});

        vk::RenderingAttachmentInfo colorAttachment =
            init::GetColorAttachmentInfo(m_Swapchain->GetImageView(frame.swapChainImageIndex),
                                         m_Swapchain->GetFormat(),
                                         vk::ImageLayout::eColorAttachmentOptimal,
                                         clearValue);

        vk::Rect2D renderArea = {{0, 0}, m_Swapchain->GetExtent()};

        vk::RenderingInfo renderInfo = init::GetRenderingInfo(renderArea, &colorAttachment, nullptr);

        utils::TransitionImageLayout(cmdBuffer,
                                     m_Swapchain->GetImage(frame.swapChainImageIndex),
                                     vk::ImageLayout::eUndefined,
                                     vk::ImageLayout::eColorAttachmentOptimal,
                                     1);

        cmdBuffer.beginRendering(renderInfo);
        cmdBuffer.setViewport(
            0, {vk::Viewport(0.0f, 0.0f, m_Swapchain->GetWidth(), m_Swapchain->GetHeight(), 0.0f, 1.0f)});
        cmdBuffer.setScissor(0, {vk::Rect2D({0, 0}, m_Swapchain->GetExtent())});

        m_RenderGraph->Execute();
        cmdBuffer.endRendering();

        utils::TransitionImageLayout(cmdBuffer,
                                     m_Swapchain->GetImage(frame.swapChainImageIndex),
                                     vk::ImageLayout::eColorAttachmentOptimal,
                                     vk::ImageLayout::ePresentSrcKHR,
                                     1);
        // end render pass

        cmdBuffer.end();

        EndFrame();
    }

    FrameData& Renderer::GetCurrentFrameData() { return m_Frames[m_FrameCounter % global::kMaxFramesInFlight]; }

    void Renderer::BeginFrame()
    {
        // begin frame
        auto       vkDevice = m_Device->GetDevice();
        FrameData& frame    = GetCurrentFrameData();

        auto result = vkDevice.waitForFences(frame.inFlightFence, true, UINT64_MAX);
        if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to wait for fence");
        }

        m_ActivePassRoot.clear();

        // reset the fence
        vkDevice.resetFences(frame.inFlightFence);

        // acquire the next image
        frame.swapChainImageIndex =
            vkDevice
                .acquireNextImageKHR(m_Swapchain->GetSwapchain(), UINT64_MAX, frame.imageAvailableSemaphore, nullptr)
                .value;

        frame.commandStream->Reset();

        m_ActivePassRoot.push_back(m_GeometryPass.get());
    }

    void Renderer::EndFrame()
    {
        // submit the command buffer and present the image
        FrameData& frame     = GetCurrentFrameData();
        auto       cmdBuffer = frame.commandStream->GetCommandBuffer();

        vk::SubmitInfo submitInfo     = {};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &cmdBuffer;

        vk::Semaphore          waitSemaphores[] = {frame.imageAvailableSemaphore};
        vk::PipelineStageFlags waitStages[]     = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores    = waitSemaphores;
        submitInfo.pWaitDstStageMask  = waitStages;

        vk::Semaphore signalSemaphores[] = {frame.renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount  = 1;
        submitInfo.pSignalSemaphores     = signalSemaphores;

        m_Device->GetQueue(CommandQueueType::Graphics).queue.submit(submitInfo, frame.inFlightFence);

        // present the image
        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;

        vk::SwapchainKHR swapchains[] = {m_Swapchain->GetSwapchain()};
        presentInfo.swapchainCount    = 1;
        presentInfo.pSwapchains       = swapchains;
        presentInfo.pImageIndices     = &frame.swapChainImageIndex;

        auto res = m_Device->GetQueue(CommandQueueType::Graphics).queue.presentKHR(presentInfo);
        if (res != vk::Result::eSuccess)
        {
            WIND_CORE_ERROR("Failed to present image");
        }

        m_FrameCounter++;
    }

    void Renderer::CreateFrameData()
    {
        m_Frames.resize(global::kMaxFramesInFlight);

        vk::Device vkDevice = m_Device->GetDevice();

        for (auto& frame : m_Frames)
        {
            frame.imageAvailableSemaphore = vkDevice.createSemaphore({});
            frame.renderFinishedSemaphore = vkDevice.createSemaphore({});
            frame.inFlightFence           = vkDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
            frame.commandStream =
                m_Device->CreateResourceUnique<CommandStream>(CommandQueueType::Graphics, StreamMode::eImmdiately);
            frame.commandStream->InitRHI();
        }
    }

    void Renderer::InitRenderGraph()
    {
        // initialize the render graph
        m_RenderGraph = std::make_unique<RenderGraph>();
        // create the render graph passes
        m_GeometryPass = std::make_unique<GeometryPass>(PipelineID::Triangle, m_PipelineCache.get());
    }

    void Renderer::RegisterDeletionQueue()
    {
        // push the deletion to the main deletion queue
        m_MainDelelteQueue.PushFunction([this]() { m_Device.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_GeometryPass.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_Window.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_Swapchain.reset(); });
        m_MainDelelteQueue.PushFunction([this]() {
            m_ShaderLibrary->Destroy();
            m_ShaderLibrary.reset();
        });
        m_MainDelelteQueue.PushFunction([this]() {
            m_PipelineCache->Destroy();
            m_PipelineCache.reset();
        });

        // push the deletion queue to the main deletion queue
        m_MainDelelteQueue.PushFunction([this]() {
            vk::Device vkDevice = m_Device->GetDevice();

            for (auto& frame : m_Frames)
            {
                vkDevice.destroySemaphore(frame.imageAvailableSemaphore);
                vkDevice.destroySemaphore(frame.renderFinishedSemaphore);
                vkDevice.destroyFence(frame.inFlightFence);
                frame.commandStream->ReleaseRHI();
            }
        });
    }

    void Renderer::ProcessDirtyShaders()
    {
        if (!m_ShaderLibrary->HaveDirtyShaders())
        {
            return;
        }

        m_Device->WaitIdle();
        m_PipelineCache->Update();
        m_ShaderLibrary->AllDirtyClear();
    }
} // namespace wind