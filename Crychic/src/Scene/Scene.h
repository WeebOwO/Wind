#pragma once

#include <entt.hpp>

namespace crychic
{
    class Scene
    {
    public:
        Scene()  = default;
        ~Scene() = default;

        
    private:
        entt::registry m_registry;
    };
} // namespace crychic