#include "ScriptEngine.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

namespace crychic
{
    static ScriptSystem* g_ScriptSystem;

    ScriptSystem* GetScriptSystem() { return g_ScriptSystem; }

    struct ScriptEngineData
    {
        MonoDomain* rootDomain = nullptr;
        MonoDomain* appDomain  = nullptr;
    };

    static ScriptEngineData* s_ScriptEngineData;

    ScriptSystem::ScriptSystem(const std::string& assemblyPath) : m_assemblyPath(assemblyPath) {}

    void ScriptSystem::InitMono()
    {
        s_ScriptEngineData = new ScriptEngineData();
        mono_set_assemblies_path(m_assemblyPath.c_str());

        // create mono domain
        MonoDomain* rootDomain = mono_jit_init("CrychicJitRuntime");
        if (rootDomain == nullptr)
        {
            // todo: output error log
        }
        const char* appDomainName = "CrychicAppDomain";
        s_ScriptEngineData->rootDomain = rootDomain;
        s_ScriptEngineData->appDomain = mono_domain_create_appdomain(const_cast<char*>(appDomainName), nullptr);
        mono_domain_set(s_ScriptEngineData->appDomain, true);
    }

    void ScriptSystem::Init()
    {
        InitMono();
        g_ScriptSystem = this;
    }

    void ScriptSystem::Quit() { delete s_ScriptEngineData; }

} // namespace crychic