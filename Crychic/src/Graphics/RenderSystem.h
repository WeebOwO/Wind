#pragma once

#include "Context.h"
#include "Core/SubSystem.h"
#include "Renderer.h"

namespace crychic
{
    // rendering engine class
    // convert the render graph to a job
    class RenderSystem : public SubSystem
    {
    public:
        explicit RenderSystem(wind::Context* context);

        void Init() override;
        void Quit() override;

        void Tick();

    private:
        wind::Context*            m_context;
        std::unique_ptr<Renderer> m_renderer;
    };

    extern RenderSystem* g_renderSystem;
} // namespace crychic
