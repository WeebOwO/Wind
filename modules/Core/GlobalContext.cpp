#include "GlobalContext.h"
#include "Log.h"

namespace wind 
{
    void GlobalContext::Init()
    {
        Log::Init();
    }

    void GlobalContext::Shutdown()
    {
        Log::Shutdown();
    }
}