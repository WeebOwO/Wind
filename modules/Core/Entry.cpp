#include "Graphics/Renderer.h"

auto main(int argc, char** argv, char** env) -> int
{
    // create a renderer
    auto renderer = wind::Renderer();
    renderer.Run();
    return 0;
}