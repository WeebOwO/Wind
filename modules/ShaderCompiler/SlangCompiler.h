#include <slang.h>

namespace wind 
{
    class SlangCompiler
    {
    public:
        SlangCompiler();
        ~SlangCompiler();

        void Compile(const char* source, const char* entryPoint, const char* target, const char* output);
        void Test();

    private:
        slang::ISession* m_Session;
    };
}