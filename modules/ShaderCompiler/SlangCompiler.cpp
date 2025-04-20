#include "SlangCompiler.h"

#include <iostream>
#include <string>

#include "Core/Log.h"

using namespace Slang;

namespace wind
{
    void diagnoseIfNeeded(slang::IBlob* diagnosticsBlob)
    {
        if (diagnosticsBlob != nullptr)
        {
            std::cout << (const char*)diagnosticsBlob->getBufferPointer() << std::endl;
        }
    }

    const char* shortestShader = "RWStructuredBuffer<float> result;"
                                 "[shader(\"compute\")]"
                                 "[numthreads(1,1,1)]"
                                 "void computeMain(uint3 threadId : SV_DispatchThreadID)"
                                 "{"
                                 "    result[threadId.x] = threadId.x;"
                                 "}";

    SlangCompiler::SlangCompiler() { InitializeSlang(); }

    SlangCompiler::~SlangCompiler() {}

    void SlangCompiler::InitializeSlang()
    {
        ComPtr<slang::ISession> slangSession;
        // Create a session
        slang::createGlobalSession(m_GlobalSession.writeRef());
        return;
    }

    // compile the shortest shader
    void SlangCompiler::Test()
    {
        slang::SessionDesc sessionDesc {};
        slang::TargetDesc  targetDesc {};

        targetDesc.format  = SLANG_SPIRV;
        targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

        sessionDesc.targets     = &targetDesc;
        sessionDesc.targetCount = 1;

        std::array<slang::CompilerOptionEntry, 1> options = {
            {slang::CompilerOptionName::EmitSpirvDirectly,
             {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}}};

        sessionDesc.compilerOptionEntries    = options.data();
        sessionDesc.compilerOptionEntryCount = options.size();

        Slang::ComPtr<slang::ISession> session;
        m_GlobalSession->createSession(sessionDesc, session.writeRef());

        // load module
        Slang::ComPtr<slang::IModule> slangModule;
        {
            Slang::ComPtr<slang::IBlob> diagBlob;
            slangModule = session->loadModuleFromSourceString(
                "shortestShader", "shortest.slang", shortestShader, diagBlob.writeRef());
            diagnoseIfNeeded(diagBlob.get());
        }

        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            slangModule->findEntryPointByName("computeMain", entryPoint.writeRef());
            if (!entryPoint)
            {
                std::cout << "Error getting entry point" << std::endl;
            }
        }

        std::array<slang::IComponentType*, 2> componentTypes = {slangModule, entryPoint};

        Slang::ComPtr<slang::IComponentType> composedProgram;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult                 result = session->createCompositeComponentType(
                componentTypes.data(), componentTypes.size(), composedProgram.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
        }

        Slang::ComPtr<slang::IComponentType> linkedProgram;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult result = composedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
        }

        Slang::ComPtr<slang::IBlob> spirvCode;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult                 result =
                linkedProgram->getEntryPointCode(0, 0, spirvCode.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
        }

        WIND_CORE_INFO("SPIRV code size: {0}", spirvCode->getBufferSize());

        return;
    }
} // namespace wind