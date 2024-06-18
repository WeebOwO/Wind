#include "Window.h"

namespace crychic
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title) :
        m_width(width), m_height(height), m_title(title)
    {}

    bool Window::ShouldClose() const { return glfwWindowShouldClose(m_window); }

    void Window::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
    }

    void Window::Update()
    {
        glfwPollEvents();
    }
} // namespace crychic
