#pragma once

#include <string>
#include <vector>

#include <slang-com-helper.h>
#include <slang-com-ptr.h>
#include <slang.h>

namespace wind
{
    class Device;

    struct VertexAndFragmentCompileEntry
    {
        std::string shaderFileName;
        std::string shaderSource;

        std::string vertexEntry;
        std::string fragmentEntry;

        std::vector<uint32_t> vertexSpirv;
        std::vector<uint32_t> fragmentSpirv;
    };

    class SlangCompiler
    {
    public:
        SlangCompiler();
        ~SlangCompiler();
    
        void Compile(VertexAndFragmentCompileEntry& entry);
        
        // 辅助方法：从文件加载着色器源码
        static std::string LoadShaderSourceFromFile(const std::string& filePath);
        
        // 便捷方法：直接从文件路径编译着色器
        static bool CompileFromFile(const std::string& filePath, 
                                   const std::string& vertexEntry,
                                   const std::string& fragmentEntry,
                                   std::vector<uint32_t>& vertexSpirv,
                                   std::vector<uint32_t>& fragmentSpirv);

    private:
        void InitializeSlang();
    
        Slang::ComPtr<slang::IGlobalSession> m_GlobalSession;
    };
} // namespace wind