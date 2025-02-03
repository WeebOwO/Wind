#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace wind
{
    class Window
    {
    public:
        Window(const std::string& string, uint32_t width, uint32_t height);
        ~Window();

        void Update();

        void Init();
        void SetVSync(bool enabled);
        bool IsVSync() const;

        GLFWwindow* GetNativeWindow() const { return m_window; }
        bool        ShouldClose() const;

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

    private:
        GLFWwindow* m_window;
        std::string m_title;
        uint32_t    m_width;
        uint32_t    m_height;
        bool        m_shouldClose;
        bool        m_vSync;
    };
} // namespace wind