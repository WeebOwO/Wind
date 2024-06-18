#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace crychic
{
    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string& title);
        bool ShouldClose() const;

        void Init();
        void Update();

        [[nodiscard]] auto GetGLFWWindow() const { return m_window; }

        [[nodiscard]] auto GetWidth() const { return m_width; }
        [[nodiscard]] auto GetHeight() const { return m_height; }
        
    private:
        GLFWwindow* m_window;
        uint32_t    m_width;
        uint32_t    m_height;
        std::string m_title;
    };
} // namespace crychic