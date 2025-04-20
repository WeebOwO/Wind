#include "Game.h"

#include <iostream>

auto main(int argc, char** argv, char** env) -> int
{
    // create a renderer
    wind::Game game;

    wind::CommandLineArguments args;
    args.argc = argc;
    args.argv = argv;

    game.ParseCommandLine(args);

    wind::ApplicationGuard<wind::Game> app(&game);
    app.Run();
    return 0;
}