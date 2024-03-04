#pragma once

#include "std.h"

#include "Backend/Command.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "nlohmann/json_fwd.hpp"

namespace wind::io
{
    // todo: move this part in io thread
    class Scene;

    template<typename T>
    std::vector<T> LoadBinary(std::string_view fileName)
    {
        std::ifstream file(fileName.data(), std::ios::binary);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", fileName);
        }

        std::vector<char> spirv((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

        file.close();

        // Copy data from the char-vector to a new uint32_t-vector
        std::vector<T> spv(spirv.size() / sizeof(T));
        memcpy(spv.data(), spirv.data(), spirv.size());

        return spv;
    }

    std::string    ReadFileToString(const std::filesystem::path& filePath);
    nlohmann::json LoadJson(std::filesystem::path filePath);

    class AsynchronousLoader
    {
    public:
        NO_COPYABLE(AsynchronousLoader);

        static AsynchronousLoader& Get() { return *s_instance; }
        static void                Init(const vk::Device& device);
        static void                Quit();

        bool LoadGLTFScene(Scene& scene, const std::filesystem::path& filePath);

    private:
        AsynchronousLoader(const vk::Device device);
        vk::Device        m_device;
        vk::CommandBuffer m_commandBuffer;
        vk::CommandPool   m_commandPool;

        inline static AsynchronousLoader* s_instance = nullptr;
    };
} // namespace wind::io