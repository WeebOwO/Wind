#pragma once

#include <cstdint>
#include <string>

#include "Core/Handle.h"
#include "Core/Singleton.h"
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
        
    private:
        void Init() override;
        void Destroy() override;
    };

    struct SceneCreateInfo
    {
        std::string sceneName;
    };

    class SceneManager : public Singleton<SceneManager>
    {
    public:
        SceneManager()          = default;
        virtual ~SceneManager() = default;

        SceneHandle CreateScene(const SceneCreateInfo& createInfo);

    private:
        wind::vector<Scene*> m_scenes;
    };
} // namespace wind