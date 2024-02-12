#pragma once

#include "Core/Script.h"

#include "Backend/ImGuiContext.h"
#include "Backend/VulkanHeader.h"

#include "Resource/Mesh.h"

#include "Renderer/ShaderMap.h"

namespace vk
{
    class ShaderModule;
};

namespace wind
{
    // manage runtime resource and system
    class GPUDevice;
    class Scene;
    class Renderer;

    struct PathManager
    {
        std::filesystem::path projectPath;
        std::filesystem::path shaderPath;
        std::filesystem::path configPath;
        std::filesystem::path asssetPath;
        std::filesystem::path pipelinePath;
    };

    class RuntimeContext
    {
    public:
        void Init(const Window& window);
        void Quit();

        std::filesystem::path GetPipelinePath(const std::string& name);

        Scope<GPUDevice> device;
        Scope<Renderer>  renderer;
        PathManager      pathManager;
    };

    std::filesystem::path GetPath(std::filesystem::path path);

    extern RuntimeContext g_runtimeContext; // last entry to access the global info, try not use this
} // namespace wind
