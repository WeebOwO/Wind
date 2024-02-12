#pragma once

#include <GLFW/glfw3.h>

#include "std.h"

namespace wind
{
    class Swapchain;
    class Window
    {
    public:
        struct WindowDesc
        {
            uint32_t    width;
            uint32_t    height;
            std::string windowTitle;
            bool        vsync = true;
        };

        Window(const WindowDesc& desc);
        ~Window();

        [[nodiscard]] auto window() const { return m_window; }
        [[nodiscard]] auto width() const { return m_windowInfo.width; }
        [[nodiscard]] auto height() const { return m_windowInfo.height; }
        [[nodiscard]] auto IsVsyncEnable() const { return m_vsync; }

        void OnUpdate(float fs);

    private:
        struct WindowInfo
        {
            uint32_t    width, height;
            std::string title;
        };

        GLFWwindow* m_window {nullptr};
        WindowInfo  m_windowInfo {};
        bool        m_vsync;
    };
} // namespace wind
