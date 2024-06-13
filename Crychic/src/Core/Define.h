#pragma once

namespace crychic
{
#define NON_COPYABLE(ClassName) \
    ClassName(ClassName const&)            = delete; \
    ClassName& operator=(ClassName const&) = delete;

#define NON_MOVABLE(ClassName) \
    ClassName(ClassName&&) noexcept            = delete; \
    ClassName& operator=(ClassName&&) noexcept = delete;

#define MOVE_ONLY(ClassName) \
    NON_COPYABLE(ClassName) \
    NON_MOVABLE(ClassName)

#define SERVICE(ClassName) \
    static void ClassName::Init(); \
    static void ClassName::Quit();

    constexpr int kFrameOverlap = 2;
} // namespace crychic