#include "Blackboard.h"

namespace wind
{
    Blackboard::Blackboard() noexcept  = default;
    Blackboard::~Blackboard() noexcept = default;

    RenderGraphHandle& Blackboard::operator[](std::string_view key) noexcept
    {
        auto [pos, _] = m_Container.insert_or_assign(key, RenderGraphHandle {});
        return pos->second;
    }

    void Blackboard::Put(std::string_view key, RenderGraphHandle value) noexcept { m_Container[key] = value; }

    void Blackboard::Remove(std::string_view key) noexcept { m_Container.erase(key); }

    RenderGraphHandle Blackboard::Get(std::string_view key) noexcept
    {
        auto it = m_Container.find(key);
        if (it != m_Container.end())
        {
            return it->second;
        }
        return RenderGraphHandle {};
    }
} // namespace wind