#pragma once

#include "RenderGraphHandle.h"
#include <string_view>
#include <unordered_map>

namespace wind
{
    class Blackboard
    {
    public:
        using Container = std::unordered_map<std::string_view, RenderGraphHandle>;

        Blackboard() noexcept;
        ~Blackboard() noexcept;

        RenderGraphHandle& operator[](std::string_view key) noexcept;

        void Put(std::string_view key, RenderGraphHandle value) noexcept;
        void Remove(std::string_view key) noexcept;

        RenderGraphHandle Get(std::string_view key) noexcept;
    private:
        Container m_Container;
    };
} // namespace wind