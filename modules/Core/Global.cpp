#include "Global.h"

#include "DeletionQueue.h"

namespace wind::global
{
    SceneManager* sceneManager;
    SlangCompiler* slangCompiler;

    static DeletionQueue deletionQueue;

    template<typename T, typename... Args>
    T* CreateGlobalObject(Args&&... args)
    {
        T* object = new T(std::forward<Args>(args)...);
        deletionQueue.PushFunction([object] { delete object; });
        return object;
    }

    void Init() 
    {
        sceneManager = CreateGlobalObject<SceneManager>();
        slangCompiler = CreateGlobalObject<SlangCompiler>();
    }

    void Shutdown() 
    { 
        deletionQueue.Flush();
    }

} // namespace wind::global