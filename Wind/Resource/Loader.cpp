#include "Loader.h"

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
} // namespace wind::io