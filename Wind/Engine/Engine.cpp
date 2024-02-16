#include "Engine.h"

#include "taskflow/core/executor.hpp"
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_vulkan.h>
#include <taskflow/taskflow.hpp>
#include <tracy/Tracy.hpp>

#include "Core/Log.h"
#include "Core/PathManager.h"
#include "Core/Script.h"
#include "ECS/Component.h"
#include "Ecs/Entity.h"
#include "Engine/Engine.h"
#include "Renderer/Material.h"
#include "Renderer/RenderGraph/RenderGraphPass.h"
#include "Renderer/RenderGraph/ResourceRegistry.h"
#include "Renderer/Renderer.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/View.h"
#include "Resource/Mesh.h"
#include "Scene/Scene.h"
#include "Window.h"

namespace wind
{
    static void ScriptTest() {
        // we already test lua in previous commit
    };

    void TA() { std::cout << "TaskA\n"; }

    static void TaskFlowTest()
    {
        // test task flow feature
        tf::Taskflow taskflow;
        tf::Executor executor;

        auto [A, B, C, D] = taskflow.emplace( // create four tasks
            TA,
            []() { std::cout << "TaskB\n"; },
            []() { std::cout << "TaskC\n"; },
            []() { std::cout << "TaskD\n"; });

        A.precede(B, C);
        D.succeed(B, C);

        executor.run(taskflow).wait();
    }

    Engine::Engine(Scope<Window> window) : m_window(std::move(window))
    {
        Init();
        PostInit();
    }

    Engine::~Engine() { Quit(); }

    void Engine::LoadScene()
    {
        auto& renderer     = Renderer::Get();
        m_activeSceneIndex = 0;
        m_scenes.push_back(scope::Create<Scene>());

        auto& scene = m_scenes[m_activeSceneIndex];

        scene->Init();
        // auto gameobject = scene->CreateGameObject("Test");
        // auto tag        = gameobject.GetComponent<TagComponent>();

        // Ref<StaticMesh> mesh = ref::Create<StaticMesh>();

        // // init the triangle
        // StaticMeshVertexFactory::Vertex v1, v2, v3;
        // v1.position = {0.0f, -0.5f, 0.0f};
        // v2.position = {0.5f, 0.5f, 0.0f};
        // v3.position = {-0.5f, 0.5f, 0.0f};

        // mesh->meshSource.vertices = {v1, v2, v3};
        // mesh->meshSource.indices  = {{0, 1, 2}};

        // // todo: this is stupid, need improve in future
        // auto materialManager = renderer.GetMaterialManager();
        // mesh->material       = materialManager->GetMaterial("default_lit").get();
        // mesh->InitRHI();

        // gameobject.AddComponent<MeshComponent>(mesh);
    }

    void Engine::Run()
    {
#ifdef TRACY_ENABLE
        WIND_CORE_INFO("Start Tracy");
#endif
        TaskFlowTest();
        ScriptTest();
        LoadScene();

        while (!glfwWindowShouldClose(m_window->window()))
        {
            ZoneScoped;
            float fs = CalcDeltaTime();
            LogicTick(fs);
            RenderTick(fs);
            FrameMark;
        }
    }

    void Engine::Init()
    {
        Log::Init();
        PathManager::Init();
        GPUDevice::Init();

        RenderConfig config {.commandBufferPerThread = 3, .renderPath = RenderPath::Defer};
        Renderer::Init(*m_window, config);
    }

    float Engine::CalcDeltaTime()
    {
        float dalta;
        {
            using namespace std::chrono;
            steady_clock::time_point tickTimePoint = steady_clock::now();
            auto                     timeSpan = duration_cast<duration<float>>(tickTimePoint - m_lastTickTimePoint);
            dalta                             = timeSpan.count();

            m_lastTickTimePoint = tickTimePoint;
        }
        return dalta;
    }

    void Engine::PostInit() { m_sceneRenderer = scope::Create<SceneRenderer>(); }

    void Engine::Quit()
    {
        GPUDevice::Get().WaitIdle();

        Renderer::Quit();
        PathManager::Quit();
        GPUDevice::Quit();
        WIND_CORE_INFO("Shutdown engine");
    }

    void Engine::RenderTick(float delta)
    {
        ZoneScopedN("RenderTick");
        // imgui start part
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // execute main render job
        auto& renderer    = Renderer::Get();
        auto& renderGraph = renderer.BeginFrame();
        // set viewport
        View view;
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // set viewport and render
        m_sceneRenderer->SetViewPort(0, 0, m_window->width(), m_window->height());
        m_sceneRenderer->Render(view, renderGraph, *m_scenes[m_activeSceneIndex]);

        auto& blackBoard = renderGraph.GetBlackBoard();

        struct PresentPassData
        {
            RenderGraphID<RenderGraphTexture> sceneColor;
        };

        renderGraph.AddPass<PresentPassData>(
            "PresentPass",
            [&](RenderGraph::Builder& builder, PresentPassData& data) {
                // present pass don't need to declare render pass
                data.sceneColor = blackBoard.Get<RenderGraphTexture>("output");
                builder.AddDependResource(data.sceneColor);
            },
            [&](ResourceRegistry& resourceRegistry, PresentPassData& data, vk::CommandBuffer cb) {
                // do all the ui command in final pass
                GPUTexture* output = renderer.GetRenderGraphOutput();

                // show viewport
                ImGui::ShowDemoWindow();

                ImGui::Begin("Viewport");
                auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
                auto viewportSize   = ImGui::GetContentRegionAvail();
                output->MarkUseByImgui(viewportSize, {0, 1}, {1, 0});
                ImGui::End();

                cb.beginRendering(resourceRegistry.GetPresentRenderingInfo());
                command::RenderUI(cb);
                cb.endRendering();

                // imgui part code
                ImGuiIO& io = ImGui::GetIO();
                (void)io;
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }
            },
            PassType::Graphics);

        renderGraph.Compile();
        renderer.NextFrame(); // will do all the render job and increase frame counter
    }

    void Engine::LogicTick(float delta)
    {
        ZoneScopedN("LogicTick");
        m_window->OnUpdate(delta);

        if (auto& activeScene = m_scenes.at(m_activeSceneIndex))
        {
            activeScene->Update();
        }
    }
} // namespace wind