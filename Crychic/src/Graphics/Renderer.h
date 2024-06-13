#pragma once

namespace crychic
{
    // store the rendering resource and provide the primitive rendering interface
    class Renderer
    {
    public:
        void BeginFrame();
        void EndFrame();
    };
} // namespace crychic