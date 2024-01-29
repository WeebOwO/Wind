#include "ImGUIContext.h"

#include "Engine/RuntimeContext.h"
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_vulkan.h>

#include "Device.h"

#include "Engine/Window.h"

#include "Backend/Command.h"
#include "Backend/SwapChain.h"
#include "Backend/Utils.h"

namespace wind
{

    void ImGUIContext::Init(const GPUDevice& device, const Window& window)
    {
        VkDevice   vkdevice  = (VkDevice)device.vkDevice();
        Swapchain* swapchain = window.GetSwapChain();

        VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
                                             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
                                             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
                                             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100},
                                             {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100}};

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets                    = 100;
        pool_info.poolSizeCount              = std::size(pool_sizes);
        pool_info.pPoolSizes                 = pool_sizes;

        vkCreateDescriptorPool(vkdevice, &pool_info, nullptr, &m_imguiPool);

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
                                                          // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui_ImplGlfw_InitForVulkan(window.GetWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo;
        initInfo.Instance              = device.vkInstance();
        initInfo.PhysicalDevice        = device.physicalDevice();
        initInfo.Device                = vkdevice;
        initInfo.Queue                 = device.mainQueue();
        initInfo.QueueFamily           = device.queueIndices().mainQueueIndex.value();
        initInfo.PipelineCache         = nullptr;
        initInfo.DescriptorPool        = m_imguiPool;
        initInfo.MinImageCount         = 3;
        initInfo.ImageCount            = 3;
        initInfo.Allocator             = nullptr;
        initInfo.MSAASamples           = VK_SAMPLE_COUNT_1_BIT;
        initInfo.ColorAttachmentFormat = VkFormat(window.GetSwapChain()->format());
        initInfo.CheckVkResultFn       = utils::CheckVkResult;
        initInfo.UseDynamicRendering   = true;

        ImGui_ImplVulkan_Init(&initInfo, nullptr);

        g_runtimeContext.device->ExecuteImmediately([](vk::CommandBuffer buffer) {
            VkCommandBuffer Cbuffer = (VkCommandBuffer)buffer;
            ImGui_ImplVulkan_CreateFontsTexture(Cbuffer);
        });

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void ImGUIContext::Quit(const GPUDevice& device)
    {
        VkDevice vkdevice = (VkDevice)device.vkDevice();
        vkDestroyDescriptorPool(vkdevice, m_imguiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }
} // namespace wind