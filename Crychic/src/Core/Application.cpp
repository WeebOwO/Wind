#include "Application.h"

#include <EngineFactoryD3D12.h>
#include <ImGuiImplWin32.hpp>
#include <imgui.h>

#include "JobSystem/JobSystem.h"
#include "Rendering/RenderSystem.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Window.h"

namespace crychic
{
    static bool showDemoWindow = true;

    static LRESULT CALLBACK MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (g_renderSystem)
        {
            auto ptr     = g_renderSystem->GetImguiPtr();
            auto handled = static_cast<ImGuiImplWin32*>(ptr)->Win32_ProcHandler(hWnd, message, wParam, lParam);
            if (handled != 0)
                return handled;
        }

        switch (message)
        {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                return 0;
            }
            case WM_SIZE: // Window size has been changed
                return 0;

            case WM_CHAR:
                if (wParam == VK_ESCAPE)
                    PostQuitMessage(0);
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_GETMINMAXINFO: {
                LPMINMAXINFO lpMMI      = (LPMINMAXINFO)lParam;
                lpMMI->ptMinTrackSize.x = 320;
                lpMMI->ptMinTrackSize.y = 240;
                return 0;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    static void ShowDemoWindow()
    {
        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);
    }

    Application::Application(const ApplicationConfig& config) : m_config(config) {}

    void Application::Run()
    {
        Init();
        MSG msg = {0};
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                g_renderSystem->NewFrame();
                auto activeScene = Singleton<SceneManager>::GetInstance().GetActiveScene();
                Update();

                m_window->Update();
                activeScene->Update(0);

                ShowDemoWindow();

                g_renderSystem->Tick();

                g_renderSystem->EndFrame();
            }
        }
    }

    void Application::RegisterWindow(HINSTANCE hinstance, int nShowCmd)
    {
        const wchar_t* windowName = L"Crychic";

        static WNDCLASSEX wcex = {sizeof(WNDCLASSEX),
                                  CS_HREDRAW | CS_VREDRAW,
                                  MessageProc,
                                  0L,
                                  0L,
                                  hinstance,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  windowName,
                                  NULL};

        RegisterClassEx(&wcex);

        m_window = std::make_unique<Window>(m_config.width, m_config.height, m_config.title);
        m_window->Init(hinstance);

        ShowWindow(m_window->GetNativeWindow(), nShowCmd);
        UpdateWindow(m_window->GetNativeWindow());
    }

    void Application::LoadScene(const std::string& path)
    {
        auto scene      = Singleton<SceneManager>::GetInstance().CreateScene("DemoScene");
        auto testEntity = scene->CreateEntity("testEntity");
        scene->SetActive(true);
    }

    void Application::Init()
    {
        m_subSystems[System::Job]    = std::make_unique<JobSystem>();
        m_subSystems[System::Render] = std::make_unique<RenderSystem>(m_window.get());

        for (auto& subSystem : m_subSystems)
        {
            if (subSystem != nullptr)
                subSystem->Init();
        }
    }

    void Application::Quit()
    {
        std::reverse(m_subSystems.begin(), m_subSystems.end());
        for (auto& subSystem : m_subSystems)
        {
            if (subSystem != nullptr)
                subSystem->Quit();
        }
    }

    Application::~Application() = default;
} // namespace crychic