#pragma once

#include <filesystem>
#include <string>

namespace crychic::io
{
    std::string LoadFileAsString(const std::filesystem::path& path);


}