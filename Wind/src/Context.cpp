#include <Context.h>

#include <iostream>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace wind {
    static Context* s_root = nullptr;
    static vk::DebugUtilsMessengerEXT s_debugMessenger;

    Context::Context(const std::vector<Feature>& featureList, bool enableValidationLayers, const surfaceCreateFunc& surfaceFunc)
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        vkb::InstanceBuilder builder;
        auto instRet = builder.set_app_name("Wind")
            .request_validation_layers(enableValidationLayers)
            .use_default_debug_messenger()
            .build();

        s_debugMessenger = instRet.value().debug_messenger;

        if(!instRet)
        {
            std::cerr << "Failed to create Vulkan instance: " << instRet.error().message() << std::endl;
        }

        // craete vulkan instance
        vkb::Instance vkbInst = instRet.value();
        m_instance            = vkbInst.instance;

        m_surface = surfaceFunc(m_instance);
        vkb::PhysicalDeviceSelector selector{vkbInst};

        vk::PhysicalDeviceVulkan12Features features12;
        features12.setBufferDeviceAddress(true)
                  .setDescriptorIndexing(true);

        vk::PhysicalDeviceVulkan13Features features13;

        std::vector<const char*> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        for(const auto feature : featureList)
        {
            switch(feature)
            {
                case Feature::MeshShader:
                    extensions.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
                    break;
                case Feature::RayTracing:
                    extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
                    extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
                    extensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
                    break;
                case Feature::DynamicRendering:
                    features13.setDynamicRendering(true);
                    break;
                case Feature::Sync2:
                    features13.setSynchronization2(true);
                    break;
            }
        }

        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_surface(m_surface)
            .add_required_extensions(extensions)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .select()
            .value();

        m_physicalDevice = physicalDevice.physical_device;

        vkb::Device device = vkb::DeviceBuilder{physicalDevice}
            .build()
            .value();

        m_device = device.device;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

        CreateQueue();
    }

    Context::~Context()
    {
        m_instance.destroy(m_surface);
        m_instance.destroy(s_debugMessenger);
        m_device.destroy();
        m_instance.destroy();
    }

    void Context::CreateQueue()
    {
        std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();

        for(uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                m_graphicsQueue.familyIndex = i;
                m_graphicsQueue.queue = m_device.getQueue(i, 0);
            }

            if(m_physicalDevice.getSurfaceSupportKHR(i, m_surface))
            {
                m_presentQueue.familyIndex = i;
                m_presentQueue.queue = m_device.getQueue(i, 0);
            }

            if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)
            {
                m_computeQueue.familyIndex = i;
                m_computeQueue.queue = m_device.getQueue(i, 0);
            }
        }
    }

    vk::Queue Context::GetQueue(wind::QueueType queueType) const
    {
        switch(queueType)
        {
            case wind::QueueType::Graphics:
                return m_graphicsQueue.queue;
            case wind::QueueType::Present:
                return m_presentQueue.queue;
            case wind::QueueType::Compute:
                return m_computeQueue.queue;
        }
    }

    void Init(GLFWwindow* window) {
        if(s_root == nullptr)
        {
            std::vector<Feature> features = {Feature::DynamicRendering, Feature::Sync2};
            auto surfaceFunc = [=](vk::Instance instance) -> vk::SurfaceKHR {
                VkSurfaceKHR surface;
                if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
                {
                    std::cerr << "Failed to create window surface!" << std::endl;
                }

                return vk::SurfaceKHR(surface);
            };

            s_root = new Context(features, true, surfaceFunc);
        }
    }

    std::shared_ptr<class SwapChain> Context::CrateSwapChain(const SwapChainDesc& swapChainDesc)
    {
        return nullptr;
    }

    void Quit() {
        if(s_root != nullptr)
        {
            delete s_root;
            s_root = nullptr;
        }
    }

    Context* GetContext() {
        assert(s_root != nullptr && "Root is not initialized!");
        return s_root;
    }
}