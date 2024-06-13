#include "Core/Application.h"

#include "JobSystem/JobSystem.h"
#include "Graphics/RenderSystem.h"

namespace crychic
{
    Application::Application(const ApplicationDesc& config)
    {
        m_window = std::make_unique<Window>(config.width, config.height, config.title);
        m_window->Init();
    }

    void Application::Run()
    {
        Init();

        auto A = g_jobSystem->Emplace([]() {std::cout << "A\n";});

        while (!m_window->ShouldClose())
        {
            m_window->Update();
            g_jobSystem->Tick();
        }
        return;
    }

    void Application::Init()
    {
        wind::Init(m_window->GetGLFWWindow());
        m_subSystems[System::Job]    = std::make_unique<JobSystem>();
        m_subSystems[System::Render] = std::make_unique<RenderSystem>(wind::GetContext());

        for (auto& subSystem : m_subSystems)
        {
            if (subSystem != nullptr)
                subSystem->Init();
        }
    }

    void Application::Quit()
    {
        wind::Quit();
        for (auto& subSystem : m_subSystems)
        {
            if (subSystem != nullptr)
                subSystem->Quit();
        }
    }

    Application::~Application() { Quit(); }
} // namespace crychic