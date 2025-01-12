#include "SlangCompiler.h"

#include <fstream>
#include <string>


#include "Core/Path.h"
#include "Core/Vector.h"

using namespace Slang;

namespace
{
    void TestSlang() {}
} // namespace

namespace wind
{
    SlangCompiler::SlangCompiler() { InitializeSlang(); }

    SlangCompiler::~SlangCompiler() {}

    void SlangCompiler::Compile()
    {
        return;
    }

    void SlangCompiler::InitializeSlang()
    {
        ComPtr<slang::ISession> slangSession;
        // Create a session
        slang::createGlobalSession(m_GlobalSession.writeRef());

        // slang::SessionDesc sessionDesc;
        // slang::TargetDesc  targetDesc;

        // targetDesc.format  = SLANG_SPIRV;
        // targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");
        // targetDesc.flags   = 0;

        // sessionDesc.targets     = &targetDesc;
        // sessionDesc.targetCount = 1;

        // wind::vector<slang::CompilerOptionEntry> options;
        // options.push_back({slang::CompilerOptionName::EmitSpirvDirectly,
        //                    {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}});

        // sessionDesc.compilerOptionEntries    = options.data();
        // sessionDesc.compilerOptionEntryCount = options.size();

        // m_GlobalSession->createSession(sessionDesc, slangSession.writeRef());

        // // create slang module
        // slang::IModule* slangModule = nullptr;
        // {
        //     ComPtr<slang::IBlob>  diagnosticBlob;
        //     std::filesystem::path shaderRootDir = wind::path::GetShaderRootDir();
        //     std::filesystem::path shaderPath    = shaderRootDir / "CPU.slang";

        //     slangModule = slangSession->loadModule(shaderPath.string().c_str(), diagnosticBlob.writeRef());
        // }

        // ComPtr<slang::IEntryPoint> entryPoint;
        // slangModule->findEntryPointByName("computeMain", entryPoint.writeRef());

        // wind::vector<slang::IComponentType*> componentTypes;
        // componentTypes.push_back(slangModule);
        // componentTypes.push_back(entryPoint);

        // ComPtr<slang::IComponentType> linkedProgram;
        // {
        //     ComPtr<slang::IBlob> diagnosticsBlob;
        //     SlangResult          result = slangSession->createCompositeComponentType(
        //         componentTypes.data(), componentTypes.size(), linkedProgram.writeRef(), diagnosticsBlob.writeRef());
        // }

        // ComPtr<slang::IBlob> sprivBlob;
        // {
        //     ComPtr<slang::IBlob> diagnosticsBlob;
        //     SlangResult          result =
        //         linkedProgram->getEntryPointCode(0, 0, sprivBlob.writeRef(), diagnosticsBlob.writeRef());
        // }

        // // write to file
        // std::filesystem::path outputDir  = wind::path::GetSPVOutputDir();
        // std::filesystem::path outputPath = outputDir / "CPU.spv";

        // if (std::filesystem::exists(outputDir) == false)
        // {
        //     std::filesystem::create_directories(outputDir);
        // }

        // std::ofstream outputFile(outputPath, std::ios::binary);
        // outputFile.write((const char*)sprivBlob->getBufferPointer(), sprivBlob->getBufferSize());
        // outputFile.close();
        return;
    }

    void SlangCompiler::Test()
    {
        TestSlang();
        return;
    }
} // namespace wind