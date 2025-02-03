#include "GlobalContext.h"
#include "Log.h"

namespace wind::global
{
    void Init()
    {
        // initialize the global context
        Log::Init();
    }

    void Shutdown()
    {
        // shutdown the global context
        Log::Shutdown();
    }

} // namespace wind::global