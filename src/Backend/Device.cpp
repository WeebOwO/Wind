#include "Backend/Device.h"

#include "spdlog/spdlog.h"
#include <VkBootstrap.h>

#include "Backend/Swapchain.h"
#include "Core/Log.h"
#include "GLFW/glfw3.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace wind
{
    Device::Device(const DeviceExtensions& extensions, Window* window) : m_extensions(extensions), m_window(window) {}
    Device::~Device() { Destroy(); }

    std::unique_ptr<Device> Device::Create(const DeviceExtensions& extensions, Window* window)
    {
        auto device = std::make_unique<Device>(extensions, window);
        device->Init();
        return device;
    }

    bool Device::Init()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        // create vk instance and physical device
        vkb::InstanceBuilder builder;
        auto                 instanceRet = builder.set_app_name("Wind")
                               .request_validation_layers(m_extensions.enableValidationLayers)
                               .require_api_version(1, 3, 0)
                               .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData) -> VkBool32 {
                                   auto severity = vkb::to_string_message_severity(messageSeverity);
                                   auto type     = vkb::to_string_message_type(messageType);
                                   printf("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
                                   return VK_FALSE;
                               })
                               .build();

        if (!instanceRet)
        {
            WIND_CORE_ERROR("Failed to create Vulkan instance");
            return false;
        }

        m_instance       = instanceRet.value();
        m_debugMessenger = instanceRet.value().debug_messenger;
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

        // create surface
        glfwCreateWindowSurface(
            VkInstance(m_instance), m_window->GetNativeWindow(), nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface));

        // create physical device
        VkPhysicalDeviceVulkan13Features features {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        // vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing  = true;

        vkb::PhysicalDeviceSelector selector {instanceRet.value()};
        vkb::PhysicalDevice         physicalDevice = selector.set_minimum_version(1, 3)
                                                 .set_surface(VkSurfaceKHR(m_surface))
                                                 .set_required_features_13(features)
                                                 .set_required_features_12(features12)
                                                 .select()
                                                 .value();

        if (!physicalDevice)
        {
            WIND_CORE_ERROR("Failed to create physical device");
            return false;
        }

        WIND_CORE_INFO("Selected physical device: {}", physicalDevice.name);

        m_physicalDevice = physicalDevice.physical_device;

        // create logical device
        vkb::DeviceBuilder deviceBuilder {physicalDevice};
        auto               device_ret = deviceBuilder.build();
        m_device                      = device_ret.value();
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

        // create queue
        auto       queueProps = m_physicalDevice.getQueueFamilyProperties();
        const auto findQueue  = [&](vk::QueueFlags flag, GPUQueue queue) {
            for (uint32_t i = 0; i < queueProps.size(); i++)
            {
                if (queueProps[i].queueFlags & flag && queue.queue == nullptr)
                {
                    queue.queue       = m_device.getQueue(i, 0);
                    queue.familyIndex = i;
                    break;
                }
            }
        };

        findQueue(vk::QueueFlagBits::eGraphics, m_mainQueue);
        findQueue(vk::QueueFlagBits::eCompute, m_asyncComputeQueue);
        findQueue(vk::QueueFlagBits::eTransfer, m_transferQueue);

        return true;
    }

    // some interface to create functions impl
    std::shared_ptr<Swapchain> Device::CreateSwapchain(const SwapchainCreateInfo& createInfo)
    {
        return CreateResource<Swapchain>(createInfo);
    }

    void Device::Destroy()
    {
        m_device.destroy();
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
        m_instance.destroySurfaceKHR(m_surface);
        m_instance.destroy();
    }
} // namespace wind