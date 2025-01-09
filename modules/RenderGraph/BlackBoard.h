#pragma once

#include <string_view>
#include <unordered_map>

#include "RenderGraphID.h"

namespace wind::rg
{
    class BlackBoard
    {
    public:
        BlackBoard() noexcept  = default;
        ~BlackBoard() noexcept = default;

        RenderGraphHandle& operator[](std::string_view key);
        void               Put(std::string_view key, RenderGraphHandle handle);
        void               Remove(std::string_view key);

        template<typename T>
        RenderGraphID<T> get(std::string_view&& name) const noexcept
        {
            return static_cast<RenderGraphID<T>>(GetHandle(std::forward<std::string_view>(name)));
        }

    private:
        RenderGraphHandle GetHandle(std::string_view name) const noexcept;

        std::unordered_map<std::string_view, RenderGraphHandle> m_handles;
    };
}; // namespace wind