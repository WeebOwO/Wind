#include "Device.h"

#include <VkBootstrap.h>

#include "Core/Log.h"

// if you want to use the default dispatch loader you need to define
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace wind
{
    Device::Device(const DeviceExtensions& extensions, Window* window) : m_Extensions(extensions), m_Window(window) {}
    Device::~Device() { Shutdown(); }

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
                               .request_validation_layers(m_Extensions.enableValidationLayers)
                               .require_api_version(1, 3, 0)
                               .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData) -> VkBool32 {
                                   // use wind log
                                   switch (messageSeverity)
                                   {
                                       case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                                           WIND_CORE_TRACE("{}", pCallbackData->pMessage);
                                           break;
                                       case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                                           WIND_CORE_INFO("{}", pCallbackData->pMessage);
                                           break;
                                       case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                                           WIND_CORE_WARN("{}", pCallbackData->pMessage);
                                           break;
                                       case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                                           WIND_CORE_ERROR("{}", pCallbackData->pMessage);
                                           break;
                                       default:
                                           break;
                                   }

                                   return VK_FALSE;
                               })
                               .build();

        if (!instanceRet)
        {
            WIND_CORE_ERROR("Failed to create Vulkan instance");
            return false;
        }

        m_Instance       = instanceRet.value();
        m_DebugMessenger = instanceRet.value().debug_messenger;
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);

        // create surface
        glfwCreateWindowSurface(
            VkInstance(m_Instance), m_Window->GetNativeWindow(), nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface));

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
                                                 .set_surface(VkSurfaceKHR(m_Surface))
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

        m_PhysicalDevice = physicalDevice.physical_device;

        // create logical device
        vkb::DeviceBuilder deviceBuilder {physicalDevice};
        auto               device_ret = deviceBuilder.build();
        m_Device                      = device_ret.value();
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);

        // create queue
        auto       queueProps = m_PhysicalDevice.getQueueFamilyProperties();
        const auto findQueue  = [&](vk::QueueFlags flag, GPUQueue& queue) {
            for (uint32_t i = 0; i < queueProps.size(); i++)
            {
                if (queueProps[i].queueFlags & flag && queue.queue == nullptr)
                {
                    queue.queue       = m_Device.getQueue(i, 0);
                    queue.familyIndex = i;
                    break;
                }
            }
        };

        findQueue(vk::QueueFlagBits::eGraphics, m_MainQueue);
        findQueue(vk::QueueFlagBits::eCompute, m_AsyncComputeQueue);
        findQueue(vk::QueueFlagBits::eTransfer, m_TransferQueue);

        // create allocator
        VmaAllocatorCreateInfo allocatorInfo {};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device         = m_Device;
        allocatorInfo.instance       = m_Instance;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        vk::CommandPoolCreateInfo poolInfo {};
        poolInfo.queueFamilyIndex = m_MainQueue.familyIndex;
        poolInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        m_ImmediateCommandPool    = m_Device.createCommandPool(poolInfo);
        m_ImmediateCommandBuffer  = m_Device.allocateCommandBuffers(vk::CommandBufferAllocateInfo {
             .commandPool        = m_ImmediateCommandPool,
             .level              = vk::CommandBufferLevel::ePrimary,
             .commandBufferCount = 1,
        })[0];

        // create internal staging buffer
        BufferDesc stagingBufferDesc = {
            .byteSize = 4 * 1024 * 1024,
            .usage    = vk::BufferUsageFlagBits::eTransferSrc,
            .memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU,
        };

        m_InternalStagingBuffer = CreateResourceUnique<GPUBuffer>(stagingBufferDesc);
        m_InternalStagingBuffer->InitRHI();

        return true;
    }

    void Device::Shutdown()
    {
        m_InternalStagingBuffer->ReleaseRHI();
        m_InternalStagingBuffer.reset();
        vmaDestroyAllocator(m_Allocator);
        m_Device.destroyCommandPool(m_ImmediateCommandPool);
        m_Device.destroy();
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger);
        m_Instance.destroySurfaceKHR(m_Surface);
        m_Instance.destroy();
    }

    GPUQueue Device::GetQueue(CommandQueueType queueType)
    {
        switch (queueType)
        {
            case CommandQueueType::Graphics:
                return m_MainQueue;
            case CommandQueueType::Compute:
                return m_AsyncComputeQueue;
            case CommandQueueType::Transfer:
                return m_TransferQueue;
            default:
                return m_MainQueue;
        }
    }

    void Device::BeginDebugRegion(vk::CommandBuffer cmdBuffer, const char* pMarkerName, const float* color)
    {
        vk::DebugUtilsLabelEXT labelInfo = {};
        labelInfo.pLabelName             = pMarkerName;
        std::array<float, 4> markCol {color[0], color[1], color[2], color[3]};
        labelInfo.color = markCol;
        cmdBuffer.beginDebugUtilsLabelEXT(labelInfo);
    }

    void Device::EndDebugRegion(vk::CommandBuffer cmdBuffer) { cmdBuffer.endDebugUtilsLabelEXT(); }

    void Device::UploadDataToGPU(const void* data, size_t size, Handle<GPUBuffer> buffer)
    {
        m_InternalStagingBuffer->Map();

        m_InternalStagingBuffer->UpdateData(data, size);

        vk::BufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size      = size,
        };

        GPUBuffer* gpuBuffer = Get(buffer);

        ExecuteImmediate([&](vk::CommandBuffer cmdBuffer) 
        {
            cmdBuffer.copyBuffer(m_InternalStagingBuffer->GetNativeHandle(), gpuBuffer->GetNativeHandle(), copyRegion);
        });

        m_InternalStagingBuffer->Unmap();
    }
} // namespace wind