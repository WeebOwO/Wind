#pragma once

#include "Core/SubSystem.h"

namespace crychic
{
    class ScriptSystem : public SubSystem
    {
    public:
        ScriptSystem(const std::string& assemblyPath);

        void Init() override;
        void Quit() override;

    private:
        void InitMono();

        std::string m_assemblyPath;
    };

    ScriptSystem* GetScriptSystem();
}