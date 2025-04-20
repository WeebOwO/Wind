#pragma once

#include "Core/Application.h"
#include "Graphics/Renderer.h"

namespace wind
{
    struct GameInfo
    {};

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
        // game data
        GameInfo m_GameInfo;
        // renderer
        std::unique_ptr<Renderer> m_Renderer;
    };
} // namespace wind