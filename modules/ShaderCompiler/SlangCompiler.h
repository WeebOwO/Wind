#pragma once

#include <string>
#include <vector>

#include <slang-com-helper.h>
#include <slang-com-ptr.h>
#include <slang.h>

namespace wind
{
    class Device;

    struct CompileEntry
    {
        std::string           entryPoint;
        std::string           target;
        std::string           sourcePath;
        std::vector<uint32_t> outSpirv;
    };

    class SlangCompiler
    {
    public:
        SlangCompiler();
        ~SlangCompiler();

        void Test();

    private:
        void InitializeSlang();
    
        Slang::ComPtr<slang::IGlobalSession> m_GlobalSession;
    };
} // namespace wind