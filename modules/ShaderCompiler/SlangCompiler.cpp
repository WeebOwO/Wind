#include "SlangCompiler.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

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

    void SlangCompiler::Compile(VertexAndFragmentCompileEntry& entry)
    {
        slang::SessionDesc sessionDesc{};
        slang::TargetDesc targetDesc{};
        
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");
        
        targetDesc.flags = SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY;
        
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        
        std::vector<slang::CompilerOptionEntry> options = {
            {slang::CompilerOptionName::EmitSpirvDirectly,
             {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}},
            {slang::CompilerOptionName::DebugInformation,
             {slang::CompilerOptionValueKind::Int, SLANG_DEBUG_INFO_LEVEL_STANDARD, 0, nullptr, nullptr}},
            {slang::CompilerOptionName::DebugInformationFormat,
             {slang::CompilerOptionValueKind::Int, SLANG_DEBUG_INFO_FORMAT_DEFAULT, 0, nullptr, nullptr}}
        };
        
        sessionDesc.compilerOptionEntries = options.data();
        sessionDesc.compilerOptionEntryCount = options.size();
        
        Slang::ComPtr<slang::ISession> session;
        m_GlobalSession->createSession(sessionDesc, session.writeRef());
        
        Slang::ComPtr<slang::IModule> slangModule;
        {
            Slang::ComPtr<slang::IBlob> diagBlob;
            slangModule = session->loadModule(entry.shaderFileName.c_str(), 
                                              diagBlob.writeRef());
            diagnoseIfNeeded(diagBlob.get());
            
            if (!slangModule) {
                WIND_CORE_ERROR("Failed to load shader module: {0}", entry.shaderFileName);
                return;
            }
        }
        
        Slang::ComPtr<slang::IEntryPoint> vertexEntryPoint;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            slangModule->findEntryPointByName(entry.vertexEntry.c_str(), vertexEntryPoint.writeRef());
            if (!vertexEntryPoint) {
                WIND_CORE_ERROR("Failed to find vertex shader entry point: {0}", entry.vertexEntry);
                return;
            }
        }
        
        Slang::ComPtr<slang::IEntryPoint> fragmentEntryPoint;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            slangModule->findEntryPointByName(entry.fragmentEntry.c_str(), fragmentEntryPoint.writeRef());
            if (!fragmentEntryPoint) {
                WIND_CORE_ERROR("Failed to find fragment shader entry point: {0}", entry.fragmentEntry);
                return;
            }
        }

        std::array<slang::IComponentType*, 3> componentTypes = {
            slangModule, vertexEntryPoint, fragmentEntryPoint
        };
        
        Slang::ComPtr<slang::IComponentType> composedProgram;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult result = session->createCompositeComponentType(
                componentTypes.data(), componentTypes.size(), 
                composedProgram.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
            
            if (SLANG_FAILED(result)) {
                WIND_CORE_ERROR("Failed to create composite component type");
                return;
            }
        }
        
        Slang::ComPtr<slang::IComponentType> linkedProgram;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;
            SlangResult result = composedProgram->link(
                linkedProgram.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
            
            if (SLANG_FAILED(result)) {
                WIND_CORE_ERROR("Failed to link shader program");
                return;
            }
        }
        
        Slang::ComPtr<slang::IBlob> vertexSpirvBlob;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;

            SlangResult result = linkedProgram->getEntryPointCode(
                0, 0, vertexSpirvBlob.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
            
            if (SLANG_FAILED(result)) {
                WIND_CORE_ERROR("Failed to get vertex shader SPIR-V code");
                return;
            }
        }
            
        Slang::ComPtr<slang::IBlob> fragmentSpirvBlob;
        {
            Slang::ComPtr<slang::IBlob> diagnosticsBlob;

            SlangResult result = linkedProgram->getEntryPointCode(
                1, 0, fragmentSpirvBlob.writeRef(), diagnosticsBlob.writeRef());
            diagnoseIfNeeded(diagnosticsBlob);
            
            if (SLANG_FAILED(result)) {
                WIND_CORE_ERROR("Failed to get fragment shader SPIR-V code");
                return;
            }
        }
        
        const uint32_t* vertexSpirvData = static_cast<const uint32_t*>(vertexSpirvBlob->getBufferPointer());
        size_t vertexSpirvSize = vertexSpirvBlob->getBufferSize() / sizeof(uint32_t);
        entry.vertexSpirv.assign(vertexSpirvData, vertexSpirvData + vertexSpirvSize);
        
        const uint32_t* fragmentSpirvData = static_cast<const uint32_t*>(fragmentSpirvBlob->getBufferPointer());
        size_t fragmentSpirvSize = fragmentSpirvBlob->getBufferSize() / sizeof(uint32_t);
        entry.fragmentSpirv.assign(fragmentSpirvData, fragmentSpirvData + fragmentSpirvSize);
        
        WIND_CORE_INFO("Successfully compiled shader: {0}", entry.shaderFileName);
        WIND_CORE_INFO("Vertex SPIR-V size: {0} words", entry.vertexSpirv.size());
        WIND_CORE_INFO("Fragment SPIR-V size: {0} words", entry.fragmentSpirv.size());
    }

    std::string SlangCompiler::LoadShaderSourceFromFile(const std::string& filePath)
    {
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            WIND_CORE_ERROR("Failed to open shader file: {0}", filePath);
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    bool SlangCompiler::CompileFromFile(const std::string& filePath, 
                                        const std::string& vertexEntry,
                                        const std::string& fragmentEntry,
                                        std::vector<uint32_t>& vertexSpirv,
                                        std::vector<uint32_t>& fragmentSpirv)
    {
        // 加载着色器源码
        std::string shaderSource = LoadShaderSourceFromFile(filePath);
        if (shaderSource.empty()) {
            return false;
        }
        
        // 创建编译条目
        VertexAndFragmentCompileEntry compileEntry;
        compileEntry.shaderFileName = filePath;
        compileEntry.shaderSource = shaderSource;
        compileEntry.vertexEntry = vertexEntry;
        compileEntry.fragmentEntry = fragmentEntry;
        
        // 编译着色器
        SlangCompiler compiler;
        compiler.Compile(compileEntry);
        
        // 检查编译结果
        if (compileEntry.vertexSpirv.empty() || compileEntry.fragmentSpirv.empty()) {
            return false;
        }
        
        // 复制结果
        vertexSpirv = compileEntry.vertexSpirv;
        fragmentSpirv = compileEntry.fragmentSpirv;
        
        return true;
    }
} // namespace wind