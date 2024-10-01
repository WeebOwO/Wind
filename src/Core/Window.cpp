#include "Core/Window.h"
#include "GLFW/glfw3.h"

namespace wind
{
    Window::Window(const std::string& string, uint32_t width, uint32_t height) :
        m_window(nullptr), m_title(string), m_width(width), m_height(height), m_shouldClose(false), m_vSync(false)
    {}

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Window::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        SetVSync(true);
    }

    void Window::Update() { glfwPollEvents(); }

    void Window::SetVSync(bool enabled)
    {
        m_vSync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    bool Window::IsVSync() const { return m_vSync; }

    bool Window::ShouldClose() const { return glfwWindowShouldClose(m_window); }
} // namespace wind