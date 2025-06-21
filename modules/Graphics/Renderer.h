#pragma once

#include <memory>

#include "Backend/Device.h"
#include "Backend/Swapchain.h"
#include "Core/Allocator.h"
#include "Core/DeletionQueue.h"
#include "Core/Service.h"
#include "Core/Window.h"
#include "PSOCache.h"
#include "Passes/GeometryPass.h"
#include "Passes/UIPass.h"
#include "ShaderCompiler/ShaderLibary.h"

namespace wind
{
    template<typename T>
    using Scoped = std::unique_ptr<T>;

    class RenderGraph;
    class View;
    class SlangCompiler;

    struct FrameData
    {
        vk::Semaphore         imageAvailableSemaphore;
        vk::Semaphore         renderFinishedSemaphore;
        vk::Fence             inFlightFence;
        DeletionQueue         deletionQueue;
        uint32_t              swapChainImageIndex;
        Scoped<CommandStream> commandStream;
    };

    // todo: this should not be a application
    // but a renderer interface
    class Renderer : public Service
    {
    public:
        Renderer();
        ~Renderer();

        void Init() override;
        void Shutdown() override;

        void Tick() override;
        void RenderFrame();

        Device* GetDevice() { return m_Device.get(); }
        void    SetRenderCamera(Camera* camera) { m_RenderCamera = camera; }
        Camera* GetRenderCamera() { return m_RenderCamera; }

    private:
        void InitView();

        void ImportRenderGraphResources();
        void InitImGui();
        void CreateFrameData();
        void ProcessDirtyShaders();
        void RegisterDeletionQueue();
        void InitRenderGraph();

        FrameData& GetCurrentFrameData();
        // life cycle
        void BeginFrame();
        void EndFrame();

        // renderer specific data
        Scoped<Window>         m_Window;
        Scoped<Device>         m_Device;
        Scoped<Swapchain>      m_Swapchain;
        Scoped<ShaderLibrary>  m_ShaderLibrary;
        Scoped<PSOCache>       m_PipelineCache;
        Scoped<RenderGraph>    m_RenderGraph;
        Scoped<SlangCompiler>  m_SlangCompiler;
        Scoped<View>           m_View;
        std::list<PassNode*>   m_ActivePassRoot;
        LinearAllocator*       m_LinearAllocator;
        DeletionQueue          m_MainDelelteQueue;
        uint32_t               m_FrameCounter;
        std::vector<FrameData> m_Frames;

        // pass specific data
        Scoped<GeometryPass> m_GeometryPass;
        Scoped<UIPass>       m_UIPass;

        Camera* m_RenderCamera;

        // imgui descriptor pool
        vk::DescriptorPool m_ImguiPool;
    };
} // namespace wind