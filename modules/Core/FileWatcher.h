#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>

namespace wind
{
    struct FileStatus
    {
        std::filesystem::path           path;
        std::filesystem::file_time_type lastWriteTime;
        bool                            isDirty = false;
    };

    class FileWatcher
    {
    public:
        FileWatcher(std::filesystem::path directory, std::regex pattern);
        ~FileWatcher() = default;

        void Update();
        auto GetWatchedFiles() const { return m_WatchedFiles; }
        void FlushDirtyFiles();

    private:
        std::regex                                  m_Pattern;
        std::filesystem::path                       m_Directory;
        std::unordered_map<std::string, FileStatus> m_WatchedFiles;
    };
} // namespace wind