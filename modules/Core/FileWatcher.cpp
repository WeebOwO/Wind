#include "FileWatcher.h"

#include "Core/Log.h"

namespace wind
{
    FileWatcher::FileWatcher(std::filesystem::path directory, std::regex pattern) :
        m_Directory(directory), m_Pattern(pattern)
    {
        for (const auto& file : std::filesystem::directory_iterator(directory))
        {
            if (!std::regex_match(file.path().string(), pattern))
            {
                continue;
            }
            m_WatchedFiles[file.path().string()] = {file.path(), std::filesystem::last_write_time(file)};
        }
    }

    void FileWatcher::Update()
    {
        for (auto& file : m_WatchedFiles)
        {
            auto& path          = file.second.path;
            auto  lastWriteTime = std::filesystem::last_write_time(path);

            if (lastWriteTime != file.second.lastWriteTime)
            {
                // file has been modified
                file.second.lastWriteTime = lastWriteTime;
                file.second.isDirty       = true;
                WIND_CORE_INFO("File modified: {0}", path.string());
            }
        }
    }

    void FileWatcher::FlushDirtyFiles()
    {
        for (auto& file : m_WatchedFiles)
        {
            if (file.second.isDirty)
            {
                file.second.isDirty = false;
            }
        }
    }

} // namespace wind