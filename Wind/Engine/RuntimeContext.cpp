#include "RuntimeContext.h"

#include "Core/Config.h"
#include "Core/Log.h"

#include "Renderer/Renderer.h"

namespace wind
{
    RuntimeContext g_runtimeContext;

    void RuntimeContext::Init(const Window& window)
    {
        // Core engine part
        Log::Init();

        // init project path
        const std::string projectDir = PROJECT_DIR;
        pathManager.projectPath      = PROJECT_DIR;
        pathManager.shaderPath       = projectDir + "\\Shaders";
        pathManager.asssetPath       = projectDir + "\\Assets";
        pathManager.pipelinePath     = pathManager.asssetPath.string() + "\\RenderPipeline";

        // init the renderer
        device = scope::Create<GPUDevice>();
        RenderConfig config {.commandBufferPerThread = 3, .renderPath = RenderPath::Defer};

        renderer = scope::Create<Renderer>(*device, config);
        renderer->Init(window);
    }

    void RuntimeContext::Quit() { renderer->Quit(); }

    std::filesystem::path RuntimeContext::GetPipelinePath(const std::string& name)
    {
        return pathManager.pipelinePath.string() + "\\" + name;
    }

    std::filesystem::path GetPath(std::filesystem::path path) { return std::filesystem::path(); }
} // namespace wind