#pragma once

#include <string>

#include <Windows.h>

namespace crychic
{
    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string& title);
        bool ShouldClose() const;

        void Init(HINSTANCE hinstance);
        void Update();

        [[nodiscard]] HWND GetNativeWindow() const { return m_window; }
        [[nodiscard]] auto GetWidth() const { return m_width; }
        [[nodiscard]] auto GetHeight() const { return m_height; }

    private:
        HWND        m_window;
        uint32_t    m_width;
        uint32_t    m_height;
        std::string m_title;
        bool        m_shouldClose;
    };
} // namespace crychic