#pragma once

namespace wind
{
#define NON_COPYABLE(T) \
    T(const T&)            = delete; \
    T& operator=(const T&) = delete;

#define NON_MOVABLE(T) \
    T(T&&)            = delete; \
    T& operator=(T&&) = delete;

#define NON_COPYABLE_AND_MOVABLE(T) \
    NON_COPYABLE(T) \
    NON_MOVABLE(T)
} // namespace wind