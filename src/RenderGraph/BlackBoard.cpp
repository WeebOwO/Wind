#include "RenderGraph/BlackBoard.h"

namespace wind::rg
{
    RenderGraphHandle& BlackBoard::operator[](std::string_view name)
    {
        auto [pos, _] = m_handles.insert_or_assign(name, RenderGraphHandle {});
        return pos->second;
    }

    void BlackBoard::Put(std::string_view name, RenderGraphHandle handle) { operator[](name) = handle; }

    void BlackBoard::Remove(std::string_view name) { m_handles.erase(name); }

    RenderGraphHandle BlackBoard::GetHandle(std::string_view name) const noexcept
    {
        if (m_handles.contains(name))
        {
            return m_handles.at(name);
        }
        return RenderGraphHandle {};
    }
}; // namespace wind::rg