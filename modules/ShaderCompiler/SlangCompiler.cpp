#include "SlangCompiler.h"

#include <string>

#include "Core/Log.h"

using namespace Slang;

namespace
{
    void TestSlang() {}
} // namespace

namespace wind
{
    SlangCompiler::SlangCompiler() { InitializeSlang(); }

    SlangCompiler::~SlangCompiler() {}

    void SlangCompiler::Compile(CompileEntry& compileEntry)
    {
        ComPtr<slang::ISession> slangSession;
        slang::SessionDesc      sessionDesc;
        slang::TargetDesc       targetDesc;

        targetDesc.format  = SLANG_SPIRV;
        targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");
        targetDesc.flags   = 0;

        sessionDesc.targets     = &targetDesc;
        sessionDesc.targetCount = 1;

        std::vector<slang::CompilerOptionEntry> options;
        options.push_back({slang::CompilerOptionName::EmitSpirvDirectly,
                           {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}});

        sessionDesc.compilerOptionEntries    = options.data();
        sessionDesc.compilerOptionEntryCount = options.size();

        m_GlobalSession->createSession(sessionDesc, slangSession.writeRef());

        slang::IModule* slangModule = nullptr;
        {
            ComPtr<slang::IBlob> diagnosticBlob;

            slangModule = slangSession->loadModule(compileEntry.sourcePath.c_str(), diagnosticBlob.writeRef());
        }

        ComPtr<slang::IEntryPoint> entryPoint;
        slangModule->findEntryPointByName(compileEntry.entryPoint.c_str(), entryPoint.writeRef());

        std::vector<slang::IComponentType*> componentTypes;
        componentTypes.push_back(slangModule);
        componentTypes.push_back(entryPoint);

        ComPtr<slang::IComponentType> linkedProgram;
        {
            ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult          result = slangSession->createCompositeComponentType(
                componentTypes.data(), componentTypes.size(), linkedProgram.writeRef(), diagnosticsBlob.writeRef());
        }

        ComPtr<slang::IBlob> sprivBlob;
        {
            ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult          result =
                linkedProgram->getEntryPointCode(0, 0, sprivBlob.writeRef(), diagnosticsBlob.writeRef());
        }

        compileEntry.outSpirv.resize(sprivBlob->getBufferSize());
        WIND_CORE_INFO("Spirv size: {0}", sprivBlob->getBufferSize());
        memcpy(compileEntry.outSpirv.data(), sprivBlob->getBufferPointer(), sprivBlob->getBufferSize());
    }

    void SlangCompiler::InitializeSlang()
    {
        ComPtr<slang::ISession> slangSession;
        // Create a session
        slang::createGlobalSession(m_GlobalSession.writeRef());
        return;
    }

    void SlangCompiler::Test()
    {
        TestSlang();
        return;
    }
} // namespace wind