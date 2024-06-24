#include "BasicIO.h"

#include <fstream>
namespace crychic::io
{
    std::string LoadFileAsString(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + path.string());
        }

        std::string content;
        file.seekg(0, std::ios::end);
        content.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(content.data(), content.size());
        file.close();

        return content;
    }

} // namespace crychic::io