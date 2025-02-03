#pragma once

#include <memory>

#include "Backend/Device.h"
#include "Backend/Swapchain.h"
#include "Core/Application.h"
#include "Core/DeletionQueue.h"
#include "Core/Window.h"
#include "ShaderCompiler/ShaderLibary.h"

#include "GeometryPass.h"

namespace wind
{
    struct FrameData
    {
        vk::Semaphore                  imageAvailableSemaphore;
        vk::Semaphore                  renderFinishedSemaphore;
        vk::Fence                      inFlightFence;
        DeletionQueue                  deletionQueue;
        uint32_t                       swapChainImageIndex;
        std::unique_ptr<CommandStream> commandStream;
    };

    class Renderer : public Application
    {
    public:
        Renderer();
        ~Renderer() override;

        void Run() override;
        void ParseCommandLine(const CommandLineArguments& args) override;

    private:
        // test function
        void DrawTriangle();
        void CreateFrameData();
        void ProcessDirtyShaders();

        FrameData& GetCurrentFrameData();
        // life cycle
        void BeginFrame();
        void EndFrame();

        // renderer specific data
        std::unique_ptr<Window>        m_Window;
        std::unique_ptr<Device>        m_Device;
        std::unique_ptr<Swapchain>     m_Swapchain;
        std::unique_ptr<ShaderLibrary> m_ShaderLibrary;
        std::vector<FrameData>         m_Frames;
        DeletionQueue                  m_MainDelelteQueue;
        uint32_t                       m_FrameCounter;
        std::unique_ptr<GeometryPass>  m_GeometryPass;
    };
} // namespace wind