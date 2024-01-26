#pragma once

#include "PsoCache.h"

#include "Core/Service.h"

#include "Backend/Command.h"
#include "Backend/Descriptor.h"
#include "Backend/Device.h"
#include "Engine/RenderConfig.h"

#include "RenderGraph/RenderGraph.h"

// define lowlevel renderer that support the whole render system
namespace wind
{
    class Swapchain;
    class PsoCache;

    class FrameParms
    {
    public:
        Ref<CommandBuffer> renderEncoder;
        Ref<CommandBuffer> computeEncoder;

        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;
        vk::Fence     flightFence;

        uint32_t swapchainImageIndex;

        Ref<DescriptorAllocator> dynamicDescriptorAllocator; // have a list of descriptor pool

    private:
        friend class Renderer;
        void Init(vk::Device device);
        void Destroy(vk::Device device);

        void ResetCommanEncoders();
    };

    // using to manage rendering resources
    class Renderer : public Service<Renderer>
    {
    public:
        Renderer(GPUDevice& device, RenderConfig config) : m_device(device), m_renderConfig(config) {}

        void Init() override;
        void Quit() override;

        // return the render graph will use this frame resource
        RenderGraph& BeginFrame(const Swapchain& swapchain);
        void         NextFrame();

        auto&       GetCurrentFrameData() { return m_frameParams[m_frameNumber]; }
        auto        GetMaterialManager() const noexcept { return m_materialManager.get(); }
        GPUTexture* GetRenderGraphOutput();

        template<typename... Args>
        uint64_t CachePso(Args&&... args)
        {
            return m_psoCache->CachePso(std::forward<Args>(args)...);
        }

        auto GetPso(uint64_t id) { return m_psoCache->GetPso(id); }
        
    private:
        void GeneratePSO(const std::string& assetPath);

        RenderConfig m_renderConfig;

        GPUDevice& m_device;
        uint32_t   m_frameNumber = 0;
        FrameParms m_frameParams[RenderConfig::MAX_FRAME_IN_FLIGHT];

        // own these resoruces
        Scope<MaterialManager> m_materialManager;
        Scope<RenderGraph>     m_renderGraph;
        Scope<ShaderMap>       m_shaderMap;
        Scope<PsoCache>        m_psoCache;

        // predefine pipeline
        vk::Pipeline m_gbufferPSO;
    };
} // namespace wind