#include <slang.h>
#include <slang-com-helper.h>
#include <slang-com-ptr.h>

namespace wind
{
    class SlangCompiler
    {
    public:
        SlangCompiler();
        ~SlangCompiler();

        void Compile();
        void Test();

    private:
        void InitializeSlang();

        Slang::ComPtr<slang::IGlobalSession> m_GlobalSession;
    };
} // namespace wind