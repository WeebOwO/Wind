/*
 *  Copyright 2019-2023 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "AndroidFileSystem.hpp"
#include "Errors.hpp"
#include "DebugUtilities.hpp"

namespace Diligent
{

namespace
{

struct AndroidFileSystemHelper
{
    static AndroidFileSystemHelper& GetInstance()
    {
        static AndroidFileSystemHelper Instance;
        return Instance;
    }

    void Init(AAssetManager* AssetManager, const char* ExternalFilesDir, const char* OutputFilesDir)
    {
        m_AssetManager     = AssetManager;
        m_ExternalFilesDir = ExternalFilesDir != nullptr ? ExternalFilesDir : "";
        m_OutputFilesDir   = OutputFilesDir != nullptr ? OutputFilesDir : "";
    }

    bool OpenFile(const char* fileName, std::ifstream& IFS, AAsset*& AssetFile, size_t& FileSize)
    {
        if (fileName == nullptr || fileName[0] == '\0')
        {
            return false;
        }

        const auto IsAbsolutePath = AndroidFileSystem::IsPathAbsolute(fileName);
        if (!IsAbsolutePath && m_ExternalFilesDir.empty() && m_AssetManager == nullptr)
        {
            LOG_ERROR_MESSAGE("File system has not been initialized. Call AndroidFileSystem::Init().");
            return false;
        }

        // First, try reading from the external directory
        if (IsAbsolutePath)
        {
            IFS.open(fileName, std::ios::binary);
        }
        else if (!m_ExternalFilesDir.empty())
        {
            auto ExternalFilesPath = m_ExternalFilesDir;
            if (ExternalFilesPath.back() != '/')
                ExternalFilesPath.append("/");
            ExternalFilesPath.append(fileName);
            IFS.open(ExternalFilesPath.c_str(), std::ios::binary);
        }

        if (IFS && IFS.is_open())
        {
            IFS.seekg(0, std::ifstream::end);
            FileSize = IFS.tellg();
            IFS.seekg(0, std::ifstream::beg);
            return true;
        }
        else if (!IsAbsolutePath && m_AssetManager != nullptr)
        {
            // Fallback to assetManager
            AssetFile = AAssetManager_open(m_AssetManager, fileName, AASSET_MODE_BUFFER);
            if (!AssetFile)
            {
                return false;
            }
            uint8_t* data = (uint8_t*)AAsset_getBuffer(AssetFile);
            if (data == nullptr)
            {
                AAsset_close(AssetFile);

                LOG_ERROR_MESSAGE("Failed to open: ", fileName);
                return false;
            }
            FileSize = AAsset_getLength(AssetFile);
            return true;
        }
        else
        {
            return false;
        }
    }

    const std::string& GetExternalFilesDir() const
    {
        return m_ExternalFilesDir;
    }

    const std::string& GetOutputFilesDir() const
    {
        return m_OutputFilesDir;
    }

private:
    AndroidFileSystemHelper() {}

private:
    AAssetManager* m_AssetManager = nullptr;
    std::string    m_ExternalFilesDir;
    std::string    m_OutputFilesDir;
};

} // namespace

bool AndroidFile::Open(const char* FileName, std::ifstream& IFS, AAsset*& AssetFile, size_t& Size)
{
    return AndroidFileSystemHelper::GetInstance().OpenFile(FileName, IFS, AssetFile, Size);
}

AndroidFile::AndroidFile(const FileOpenAttribs& OpenAttribs) :
    BasicFile{OpenAttribs}
{
    auto FullPath = m_OpenAttribs.strFilePath;
    if (!Open(FullPath, m_IFS, m_AssetFile, m_Size))
    {
        LOG_ERROR_AND_THROW("Failed to open file ", FullPath);
    }
}

AndroidFile::~AndroidFile()
{
    if (m_IFS && m_IFS.is_open())
        m_IFS.close();

    if (m_AssetFile != nullptr)
        AAsset_close(m_AssetFile);
}

bool AndroidFile::Read(IDataBlob* pData)
{
    pData->Resize(GetSize());
    return Read(pData->GetDataPtr(), pData->GetSize());
}

bool AndroidFile::Read(void* Data, size_t BufferSize)
{
    VERIFY(BufferSize == m_Size, "Only whole file reads are currently supported");

    if (m_IFS && m_IFS.is_open())
    {
        m_IFS.read((char*)Data, BufferSize);
        return true;
    }
    else if (m_AssetFile != nullptr)
    {
        const uint8_t* src_data = (uint8_t*)AAsset_getBuffer(m_AssetFile);
        off_t          FileSize = AAsset_getLength(m_AssetFile);
        if (FileSize > static_cast<off_t>(BufferSize))
        {
            LOG_WARNING_MESSAGE("Requested buffer size (", BufferSize, ") exceeds file size (", FileSize, ")");
            BufferSize = FileSize;
        }
        memcpy(Data, src_data, BufferSize);
        return true;
    }
    else
    {
        return false;
    }
}

bool AndroidFile::Write(const void* Data, size_t BufferSize)
{
    UNSUPPORTED("Not implemented");

    return false;
}

size_t AndroidFile::GetPos()
{
    UNSUPPORTED("Not implemented");

    return 0;
}

bool AndroidFile::SetPos(size_t Offset, FilePosOrigin Origin)
{
    UNSUPPORTED("Not implemented");
    return false;
}


void AndroidFileSystem::Init(struct AAssetManager* AssetManager,
                             const char*           ExternalFilesDir,
                             const char*           OutputFilesDir)
{
    AndroidFileSystemHelper::GetInstance().Init(AssetManager, ExternalFilesDir, OutputFilesDir);
}

AndroidFile* AndroidFileSystem::OpenFile(const FileOpenAttribs& OpenAttribs)
{
    AndroidFile* pFile = nullptr;
    try
    {
        pFile = new AndroidFile(OpenAttribs);
    }
    catch (const std::runtime_error& err)
    {
    }

    return pFile;
}

bool AndroidFileSystem::FileExists(const Char* strFilePath)
{
    std::ifstream   IFS;
    AAsset*         AssetFile = nullptr;
    size_t          Size      = 0;
    FileOpenAttribs OpenAttribs;
    OpenAttribs.strFilePath = strFilePath;
    BasicFile   DummyFile{OpenAttribs};
    const auto& Path   = DummyFile.GetPath(); // This is necessary to correct slashes
    bool        Exists = AndroidFile::Open(Path.c_str(), IFS, AssetFile, Size);

    if (IFS && IFS.is_open())
        IFS.close();
    if (AssetFile != nullptr)
        AAsset_close(AssetFile);

    return Exists;
}

std::string AndroidFileSystem::GetLocalAppDataDirectory(const char* AppName /*= nullptr*/, bool Create /*= true*/)
{
    const std::string& OutputFilesDir = AndroidFileSystemHelper::GetInstance().GetOutputFilesDir();
    if (OutputFilesDir.empty())
    {
        LOG_ERROR_MESSAGE("Output files directory has not been initialized. Call AndroidFileSystem::Init().");
    }
    return OutputFilesDir;
}

} // namespace Diligent
