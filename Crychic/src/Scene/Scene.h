#pragma once

#include <functional>

#include <entt.hpp>

#include "Core/UUID.h"

namespace crychic
{
    class Entity;

    class Scene
    {
    public:
        explicit Scene(const std::string& name);

        using OnSceneUpdateCallBack = std::function<void(Scene& scene, float deltaTime)>;

        Scene()  = default;
        ~Scene() = default;

        Entity CreateEntity(const std::string& name = "Unknown");

        static void RegisterOnSceneUpdateCallBack(const OnSceneUpdateCallBack& callBack);

        void Update(float ts);

    private:
        friend class Entity;
        using EntityMap = std::unordered_map<UUID, Entity>;

        entt::registry m_registry;
        std::string    m_name;
        EntityMap      m_entityMap;
    };

} // namespace crychic