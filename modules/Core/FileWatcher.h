#pragma once

#include <filesystem>
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
        FileWatcher(std::filesystem::path directory);
        ~FileWatcher() = default;

        void Start();
        void Update();

        auto GetWatchedFiles() const { return m_WatchedFiles; }

        void FlushDirtyFiles();

    private:
        std::filesystem::path                       m_Directory;
        std::unordered_map<std::string, FileStatus> m_WatchedFiles;
    };
} // namespace wind