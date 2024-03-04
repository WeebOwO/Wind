#include "Loader.h"

#include <fileapi.h>
#include <fstream>

#include "nlohmann/json.hpp"

namespace wind::io
{
    nlohmann::json LoadJson(std::filesystem::path filePath)
    {
        using json = nlohmann::json;
        json          j;
        std::ifstream jfile(filePath);
        jfile >> j;
        return j;
    }

    std::string ReadFileToString(const std::filesystem::path& filePath)
    {
        std::ifstream input_file(filePath);
        if (!input_file.is_open())
        {
            std::cerr << "Could not open the file - '" << filePath << "'" << std::endl;
            exit(EXIT_FAILURE);
        }
        return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    }

    void AsynchronousLoader::Init(const vk::Device& device) { s_instance = new AsynchronousLoader(device); }

    void AsynchronousLoader::Quit() { delete s_instance; }

    AsynchronousLoader::AsynchronousLoader(const vk::Device device) : m_device(device)
    {
        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex          = 0;
        poolInfo.flags                     = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_commandPool = m_device.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool                   = m_commandPool;
        allocInfo.commandBufferCount            = 1;

        m_commandBuffer = m_device.allocateCommandBuffers(allocInfo)[0];
    }

    bool AsynchronousLoader::LoadGLTFScene(Scene& scene, const std::filesystem::path& filePath) { 
        return false; 
    }
} // namespace wind::io