#include "Window.h"

#include "Backend/SwapChain.h"
#include "Engine/RuntimeContext.h"

namespace wind
{

    Window::Window(const WindowDesc& desc) : m_windowInfo(WindowInfo {desc.width, desc.height, desc.windowTitle})
    {
        // init the glfw context
        m_vsync = desc.vsync;
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(desc.width, desc.height, desc.windowTitle.c_str(), nullptr, nullptr);
    }

    Window::~Window() { glfwDestroyWindow(m_window); }

    void Window::OnUpdate(float fs) { glfwPollEvents(); }
} // namespace wind