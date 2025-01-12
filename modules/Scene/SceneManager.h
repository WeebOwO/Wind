#pragma once

#include <cstdint>
#include <string>

#include "Core/Handle.h"
#include "Core/Vector.h"

namespace wind
{
    class Scene;
    class SceneMananger;

    class SceneHandle : public Handle<Scene>
    {
    public:
        SceneHandle() = default;
        SceneHandle(uint32_t index) : Handle(index) {}
    };

    struct SceneCreateInfo
    {
        std::string sceneName;
    };

    class SceneManager
    {
    public:
        SceneManager()          = default;
        virtual ~SceneManager();

        SceneHandle CreateScene(const SceneCreateInfo& createInfo);
        void        DestroyScene(SceneHandle& handle);

        Scene* GetActiveScene();

        Scene* GetScene(SceneHandle handle);
        Scene* GetScene(const std::string& sceneName);

    private:
        wind::vector<Scene*> m_Scenes;
    };
} // namespace wind