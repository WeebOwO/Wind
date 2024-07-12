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
        using OnSceneActiveCallBack = std::function<void(Scene& scene, bool active)>;

        Scene()  = default;
        ~Scene() = default;

        Entity CreateEntity(const std::string& name = "Unknown");

        static void RegisterOnSceneUpdateCallBack(const OnSceneUpdateCallBack& callBack);
        static void RegisterOnSceneActiveCallBack(const OnSceneActiveCallBack& callBack);


        void Update(float ts);
        void SetActive(bool active);

    private:
        friend class Entity;
        friend class SceneManager;

        using EntityMap = std::unordered_map<UUID, Entity>;

        bool           m_active;
        entt::registry m_registry;
        std::string    m_name;
        EntityMap      m_entityMap;
    };

} // namespace crychic