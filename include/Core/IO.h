#pragma once

#include <fstream>
#include <string>

#include "Log.h"

namespace wind::io
{
    template<typename T>
    std::vector<T> ReadFile(const std::string& filename)
    {
        std::ifstream file(filename.data(), std::ios::binary);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", filename);
        }

        std::vector<char> spirv((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

        file.close();

        // Copy data from the char-vector to a new uint32_t-vector
        std::vector<T> spv(spirv.size() / sizeof(T));
        memcpy(spv.data(), spirv.data(), spirv.size());

        return spv;
    };

    inline std::string ReadFileAsString(const std::string& filename)
    {
        // Open the file and put the text to a string
        std::ifstream file(filename);

        if (!file.is_open())
        {
            WIND_CORE_ERROR("Failed to open file with file path {}!", filename);
        }

        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        return text;
    }
} // namespace wind::io