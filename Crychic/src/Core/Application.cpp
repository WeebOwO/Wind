#include "Application.h"

#include "Rendering/RenderSystem.h"
#include "JobSystem/JobSystem.h"

#include <EngineFactoryD3D12.h>

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

        while (!m_window->ShouldClose())
        {
            m_window->Update();
            g_jobSystem->Tick();
        }
        return;
    }

    void Application::Init()
    {
        m_subSystems[System::Job]    = std::make_unique<JobSystem>();
        m_subSystems[System::Render] = std::make_unique<RenderSystem>( m_window.get());

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

    Application::~Application() { Quit(); }
} // namespace crychic