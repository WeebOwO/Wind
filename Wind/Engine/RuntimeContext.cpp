#include "RuntimeContext.h"

#include "Engine/RenderConfig.h"
#include <shaderc/shaderc.hpp>

#include "Core/Config.h"
#include "Core/Log.h"
#include "Engine/RuntimeContext.h"
#include "Resource/VertexFactory.h"

#include "Backend/ComputeShader.h"
#include "Backend/Device.h"
#include "Backend/PipelineBuilder.h"
#include "Backend/RasterShader.h"
#include "Backend/Shader.h"

#include "Renderer/Material.h"
#include "Renderer/Renderer.h"

#include "Resource/Mesh.h"

namespace wind
{
    RuntimeContext g_runtimeContext;

    void RuntimeContext::Init()
    {
        // Core engine part
        Log::Init();
        device = scope::Create<GPUDevice>();

        // init the renderer
        RenderConfig config {.renderPath = RenderPath::Defer};

        renderer = scope::Create<Renderer>(*device, config);
        renderer->Init();

        // init project path
        const std::string projectDir = PROJECT_DIR;
        pathManager.projectPath      = PROJECT_DIR;
        pathManager.shaderPath       = projectDir + "\\Shaders";
        pathManager.asssetPath       = projectDir + "\\Assets";
        pathManager.pipelinePath     = pathManager.asssetPath.string() + "\\RenderPipeline";
        // init the lua virutal machine
        luaState = luaL_newstate();
        if (luaState == NULL)
        {
            WIND_CORE_ERROR("Fail to create lua machine");
        }
    }

    void RuntimeContext::Quit()
    {
        device->WaitIdle();
        renderer->Quit();
        guiContext->Quit(*device);
        lua_close(luaState);
    }

    void RuntimeContext::PostInit(const Window& window)
    {
        guiContext = scope::Create<ImGUIContext>();
        guiContext->Init(*device, window);
    }

    std::filesystem::path RuntimeContext::GetPipelinePath(const std::string& name)
    {
        return pathManager.pipelinePath.string() + "\\" + name;
    }

    std::filesystem::path GetPath(std::filesystem::path path) { return std::filesystem::path(); }
} // namespace wind