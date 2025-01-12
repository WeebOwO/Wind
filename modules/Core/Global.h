#pragma once

#include "Scene/SceneManager.h"
#include "ShaderCompiler/SlangCompiler.h"

// everything in the global namespace
// and not sync itself
namespace wind::global
{
    void Init();
    void Shutdown();

    extern SceneManager*  sceneManager;
    extern SlangCompiler* slangCompiler;
} // namespace wind::global