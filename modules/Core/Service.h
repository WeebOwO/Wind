#pragma once

#include <string>
#include <memory>

namespace wind
{
    class Application;

    enum class Signal : uint8_t
    {
        None = 0,
        NeedExit,
        NeedReload,
    };

    class Service
    {
    public:
        Service(const std::string& name);
        ~Service() = default;

        virtual void Init() {};
        virtual void Shutdown() {};

        virtual void Tick() {};

        bool IsActive() const { return m_Active; }

        void ProduceSignal(Signal signal);

        void BindApplication(Application* app) { m_Application = app; }

    protected:
        // service data
        Application* m_Application;
        std::string m_Name;
        // service state
        bool m_Active = true;
    };
} // namespace wind