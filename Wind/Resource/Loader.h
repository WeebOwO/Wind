#pragma once

#include "std.h"

#include "nlohmann/json_fwd.hpp"

#include "Core/Log.h"

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

    class GLTFLoader
    {
    public:
        void LoadGLTFScene(Scene& scene, const std::string& filePath);
    };

    std::string ReadFileToString(const std::filesystem::path& filePath);

    nlohmann::json LoadJson(std::filesystem::path filePath);

} // namespace wind::io