#pragma once

namespace wind
{
    enum SceneState
    {
        Invalid = 0,
        Loaded,
        Active,
        Unloaded
    };

    class Scene 
    {
    public:
        Scene() = default;
        virtual ~Scene() = default;

    private:
        SceneState m_state = SceneState::Invalid;
    };
} // namespace wind