#pragma once

namespace crychic
{
    class Component
    {
    public:
        virtual void PreUpdate() {}
        virtual void Update() {}
        virtual void LateUpdate() {}
        virtual void AddToManager() {}
        virtual void RemoveFromManager() {}
    };
}