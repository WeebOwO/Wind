#pragma once

#include "Core/Application.h"
#include "Graphics/Renderer.h"

namespace wind
{
    class Renderer;

    class Game : public Application
    {
    public:
        Game()           = default;
        ~Game() override = default;

        void Init() override;
        void Shutdown() override;

        void Run() override;

        void ParseCommandLine(const CommandLineArguments& args) override;

    private:
        // renderer
        std::unique_ptr<Renderer> m_Renderer;
    };
} // namespace wind