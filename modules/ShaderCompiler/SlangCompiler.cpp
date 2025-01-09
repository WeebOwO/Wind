#include "SlangCompiler.h"

#include <slang-com-ptr.h>
#include <slang-com-helper.h>

using namespace Slang;

namespace 
{
    void TestSlang() 
    {
        Slang::ComPtr<slang::ISession> slangSession;
        // Create a session
    }
}

namespace wind 
{
    SlangCompiler::SlangCompiler()
    {
     
    }

    SlangCompiler::~SlangCompiler()
    {
       
    }

    void SlangCompiler::Compile(const char* source, const char* entryPoint, const char* target, const char* output)
    {
        return;
    }

    void SlangCompiler::Test()
    {
        TestSlang();
        return;
    }
}