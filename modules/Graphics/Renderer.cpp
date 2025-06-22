#include "Renderer.h"

#include "Backend/Utils.h"
#include "Core/GlobalContext.h"
#include "Core/Log.h"
#include "GlobalRT.h"
#include "RenderGraph/RenderGraph.h"
#include "ShaderCompiler/SlangCompiler.h"
#include "View.h"

#include "Roboto-Regular.embed"

// imgui part
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

namespace wind
{
    Renderer::Renderer() : Service("Renderer"), m_FrameCounter(0)
    {
        // initialize the renderer
        m_Window = std::make_unique<Window>("Wind", 800, 600);
        m_Window->Init();

        m_View = std::make_unique<View>();

        DeviceExtensions extensions = {.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, .enableValidationLayers = true};
        m_Device                    = Device::Create(extensions, m_Window.get());
        m_Swapchain                 = m_Device->CreateResourceUnique<Swapchain>(800, 600);
        m_ShaderLibrary             = std::make_unique<ShaderLibrary>();
        m_PipelineManager           = std::make_unique<PipelineManager>();

        // initialize the persistent memory with 1MB
        m_LinearAllocator = new LinearAllocator(1024 * 1024);
    }

    void Renderer::Init()
    {
        WIND_CORE_INFO("Renderer initialized");
        // create frame data
        CreateFrameData();
        RegisterDeletionQueue();

        m_ShaderLibrary->Init(m_Device.get());
        m_PipelineManager->Init(m_Device.get(), m_ShaderLibrary.get());
        m_GPUScene = std::make_unique<GPUScene>(m_Device.get());

        // Triangle mesh
        SubMesh triangleSubMesh = {
            .vertices =
                {
                    {.position = {0.0f, -0.5f, 0.5f}, .color = {1.0f, 0.0f, 0.0f}},
                    {.position = {0.5f, 0.5f, 0.5f}, .color = {0.0f, 1.0f, 0.0f}},
                    {.position = {-0.5f, 0.5f, 0.5f}, .color = {0.0f, 0.0f, 1.0f}},
                },
            .indices = {0, 1, 2},
        };

        Mesh triangleMesh = {
            .subMeshes = {triangleSubMesh},
        };

        std::vector<DescriptorAllocator::PoolSizeRatio> globalSize = {
            {vk::DescriptorType::eUniformBuffer, 3},
            {vk::DescriptorType::eStorageBuffer, 3},
            {vk::DescriptorType::eCombinedImageSampler, 4},
        };

        m_GlobalDescriptorAllocator = m_Device->CreateResourceUnique<DescriptorAllocator>(1000, globalSize);

        m_GPUScene->UploadMesh(triangleMesh);

        // init imgui part
        InitImGui();

        InitRenderGraph();
    }

    void Renderer::InitImGui()
    {
        WIND_CORE_INFO("Init ImGui");
        // init imgui
        vk::DescriptorPoolSize poolSizes[] = {{vk::DescriptorType::eSampler, 1000},
                                              {vk::DescriptorType::eCombinedImageSampler, 1000},
                                              {vk::DescriptorType::eSampledImage, 1000},
                                              {vk::DescriptorType::eStorageImage, 1000},
                                              {vk::DescriptorType::eUniformTexelBuffer, 1000},
                                              {vk::DescriptorType::eStorageTexelBuffer, 1000},
                                              {vk::DescriptorType::eUniformBuffer, 1000},
                                              {vk::DescriptorType::eStorageBuffer, 1000},
                                              {vk::DescriptorType::eUniformBufferDynamic, 1000},
                                              {vk::DescriptorType::eStorageBufferDynamic, 1000},
                                              {vk::DescriptorType::eInputAttachment, 1000}};

        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.flags                        = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.maxSets                      = 1000;
        poolInfo.poolSizeCount                = static_cast<uint32_t>(std::size(poolSizes));
        poolInfo.pPoolSizes                   = poolSizes;

        m_ImguiPool = m_Device->GetDevice().createDescriptorPool(poolInfo);

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // init glfw imgui
        ImGui_ImplGlfw_InitForVulkan(m_Window->GetNativeWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance                  = m_Device->GetInstance();
        initInfo.PhysicalDevice            = m_Device->GetPhysicalDevice();
        initInfo.Device                    = m_Device->GetDevice();
        initInfo.QueueFamily               = m_Device->GetQueue(CommandQueueType::Graphics).familyIndex;
        initInfo.Queue                     = m_Device->GetQueue(CommandQueueType::Graphics).queue;
        initInfo.UseDynamicRendering       = true;
        initInfo.DescriptorPool            = m_ImguiPool;
        initInfo.MinImageCount             = m_Swapchain->GetImageCount();
        initInfo.ImageCount                = m_Swapchain->GetImageCount();

        initInfo.PipelineRenderingCreateInfo       = {};
        initInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;

        VkFormat        colorFormats[]                               = {(VkFormat)m_Swapchain->GetFormat()};
        const VkFormat* format                                       = (const VkFormat*)colorFormats;
        initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = format;

        ImGui_ImplVulkan_Init(&initInfo);

        // upload the font texture
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImFont* robotoFont =
            io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
        io.FontDefault = robotoFont;

        ImGui_ImplVulkan_CreateFontsTexture();

        m_MainDelelteQueue.PushFunction([this]() {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            m_Device->GetDevice().destroyDescriptorPool(m_ImguiPool);
        });
    }

    void Renderer::Shutdown()
    {
        WIND_CORE_INFO("Renderer shutting down");
        m_Device->WaitIdle();
        m_GPUScene->ReleaseScene();
        m_MainDelelteQueue.Flush();
        delete m_LinearAllocator;
    }

    Renderer::~Renderer() = default;

    void Renderer::Tick()
    {
        if (!m_Window->ShouldClose())
        {
            m_Window->Update();
            m_ShaderLibrary->Update();
            ProcessDirtyShaders();
            RenderFrame();
        }
        else
        {
            ProduceSignal(Signal::NeedExit);
        }
    }

    void Renderer::InitView()
    {
        m_View->viewport = {0, 0, m_Swapchain->GetWidth(), m_Swapchain->GetHeight()};
        m_View->camera   = m_RenderCamera;
        m_RenderGraph->SetView(m_View.get());
        // will perform the culling logic here in the future
    }

    void Renderer::RenderFrame()
    {
        // draw a triangle
        BeginFrame();

        auto& frame = GetCurrentFrameData();

        RenderGraphUpdateContext context = {
            .frameIndex    = m_FrameCounter,
            .commandStream = frame.commandStream.get(),
            .scene         = m_GPUScene.get(),
        };

        InitView();

        m_RenderGraph->PrepareFrame(context);
        // add the pass based on our need
        m_RenderGraph->AddPass(m_GeometryPass.get());
        m_RenderGraph->AddPass(m_UIPass.get());

        ImportRenderGraphResources();

        vk::CommandBuffer cmdBuffer = frame.commandStream->Begin();

        utils::TransitionImageLayout(cmdBuffer,
                                     m_Swapchain->GetImage(frame.swapChainImageIndex),
                                     vk::ImageLayout::eUndefined,
                                     vk::ImageLayout::eColorAttachmentOptimal,
                                     1);

        m_RenderGraph->Execute();

        utils::TransitionImageLayout(cmdBuffer,
                                     m_Swapchain->GetImage(frame.swapChainImageIndex),
                                     vk::ImageLayout::eColorAttachmentOptimal,
                                     vk::ImageLayout::ePresentSrcKHR,
                                     1);
        // end render pass

        cmdBuffer.end();

        EndFrame();
    }

    FrameData& Renderer::GetCurrentFrameData()
    {
        return m_Frames[m_FrameCounter % g_GlobalContext->kMaxFramesInFlight];
    }

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
        m_LinearAllocator->Reset();

        // reset the fence
        vkDevice.resetFences(frame.inFlightFence);

        // acquire the next image
        frame.swapChainImageIndex =
            vkDevice
                .acquireNextImageKHR(m_Swapchain->GetSwapchain(), UINT64_MAX, frame.imageAvailableSemaphore, nullptr)
                .value;

        frame.commandStream->Reset();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
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
        m_Frames.resize(g_GlobalContext->kMaxFramesInFlight);

        vk::Device vkDevice = m_Device->GetDevice();

        std::vector<DescriptorAllocator::PoolSizeRatio> frameSize = {
            {vk::DescriptorType::eUniformBuffer, 3},
            {vk::DescriptorType::eStorageBuffer, 3},
            {vk::DescriptorType::eCombinedImageSampler, 4},
        };

        for (auto& frame : m_Frames)
        {
            frame.imageAvailableSemaphore = vkDevice.createSemaphore({});
            frame.renderFinishedSemaphore = vkDevice.createSemaphore({});
            frame.inFlightFence           = vkDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
            frame.commandStream =
                m_Device->CreateResourceUnique<CommandStream>(CommandQueueType::Graphics, StreamMode::eImmdiately);
            frame.commandStream->InitRHI();

            frame.descriptorAllocator = m_Device->CreateResourceUnique<DescriptorAllocator>(1000, frameSize);
            frame.descriptorAllocator->InitRHI();
        }
    }

    void Renderer::ImportRenderGraphResources()
    {
        // import the render graph resources
        auto& frame = GetCurrentFrameData();

        RDGResourceDesc importImageDesc =
            RDGResourceDesc::Image2D(m_Swapchain->GetFormat(), m_Swapchain->GetWidth(), m_Swapchain->GetHeight());

        VirtualImage* virtualImage = m_LinearAllocator->Construct<VirtualImage>(importImageDesc);
        virtualImage->image        = m_Swapchain->GetImage(frame.swapChainImageIndex);
        virtualImage->imageView    = m_Swapchain->GetImageView(frame.swapChainImageIndex);
        virtualImage->imageLayout  = vk::ImageLayout::eColorAttachmentOptimal;
        virtualImage->name         = GlobalRT::BackBuffer;

        // import the swapchain image as a render graph resource
        RenderGraphHandle backBufferHandle = m_RenderGraph->ImportRenderGraphResource(virtualImage);
        m_RenderGraph->GetBlackboard().Put(GlobalRT::BackBuffer, backBufferHandle);

        // import camera color
        GPUTexture*  cameraTexture = m_Device->Get(m_RenderCamera->GetTextureHandle());
        vk::Extent3D extent        = cameraTexture->imageInfo.extent;

        RDGResourceDesc cameraImageDesc =
            RDGResourceDesc::Image2D(cameraTexture->imageInfo.format, extent.width, extent.height);

        VirtualImage* cameraVirtualImage   = m_LinearAllocator->Construct<VirtualImage>(cameraImageDesc);
        cameraVirtualImage->image          = cameraTexture->allocateImage.image;
        cameraVirtualImage->imageView      = cameraTexture->imageView;
        cameraVirtualImage->imageLayout    = vk::ImageLayout::eColorAttachmentOptimal;
        cameraVirtualImage->name           = GlobalRT::SceneColor;
        cameraVirtualImage->imported       = true;
        RenderGraphHandle sceneColorHandle = m_RenderGraph->ImportRenderGraphResource(cameraVirtualImage);
        m_RenderGraph->GetBlackboard().Put(GlobalRT::SceneColor, sceneColorHandle);
    }

    void Renderer::InitRenderGraph()
    {
        // initialize the render graph
        m_RenderGraph = std::make_unique<RenderGraph>(m_Device.get());
        // create the render graph passes
        m_GeometryPass = std::make_unique<GeometryPass>(m_PipelineManager.get());
        m_UIPass       = std::make_unique<UIPass>(m_PipelineManager.get());
    }

    void Renderer::RegisterDeletionQueue()
    {
        // push the deletion to the main deletion queue
        m_MainDelelteQueue.PushFunction([this]() { m_Device.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_GeometryPass.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_Window.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_Swapchain.reset(); });
        m_MainDelelteQueue.PushFunction([this]() { m_GlobalDescriptorAllocator->ReleaseRHI(); });
        m_MainDelelteQueue.PushFunction([this]() {
            m_ShaderLibrary->Destroy();
            m_ShaderLibrary.reset();
        });
        
        m_MainDelelteQueue.PushFunction([this]() {
            m_PipelineManager->Destroy();
            m_PipelineManager.reset();
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
                frame.descriptorAllocator->ReleaseRHI();
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
        m_PipelineManager->Update();
        m_ShaderLibrary->AllDirtyClear();
    }
} // namespace wind