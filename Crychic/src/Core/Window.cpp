#include "Window.h"

#include <iostream>

namespace crychic
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title) :
        m_width(width), m_height(height), m_title(title)
    {}

    bool Window::ShouldClose() const { return m_shouldClose; }

    void Window::Init(HINSTANCE hinstance)
    {
        LONG windowWidth  = static_cast<LONG>(m_width);
        LONG windowHeight = static_cast<LONG>(m_height);
        RECT rc           = {0, 0, windowWidth, windowHeight};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        m_window = CreateWindowEx(0,
                                  L"Crychic",
                                  L"Crychic Engine",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  rc.right - rc.left,
                                  rc.bottom - rc.top,
                                  NULL,
                                  NULL,
                                  hinstance,
                                  NULL);
        if (!m_window)
        {
            MessageBox(NULL, L"Cannot create window", L"Error", MB_OK | MB_ICONERROR);
            printf("Oh shi- %d\n", GetLastError());
        }
    }

    void Window::Update() {}
} // namespace crychic
