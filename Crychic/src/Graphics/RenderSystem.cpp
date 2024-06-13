#include "Graphics/RenderSystem.h"

namespace crychic
{
    RenderSystem* g_renderSystem = nullptr;

    void RenderSystem::Init()
    {
        // create renderer
        m_renderer      = std::make_unique<Renderer>();
        m_isInitialized = true;
        assert(g_renderSystem == nullptr);
        g_renderSystem = this;
    }

    void RenderSystem::Quit()
    {
        m_renderer.reset();
    }

    RenderSystem::RenderSystem(wind::Context* context) : m_context(context) {}

    void RenderSystem::Tick()
    {
        // render
    }

} // namespace crychic