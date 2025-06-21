#pragma once

#include <string>

namespace wind 
{
    struct FastPropertyName
    {
        FastPropertyName(const std::string& name) : m_Name(name), m_Hash(std::hash<std::string>()(name)) {}

        const std::string& GetName() const { return m_Name; }

        operator const std::string&() const { return m_Name; }

        bool operator==(const FastPropertyName& other) const { return m_Hash == other.m_Hash; }
        bool operator==(const std::string& other) const { return m_Hash == std::hash<std::string>()(other); }
        bool operator==(const char* other) const { return m_Hash == std::hash<std::string>()(other); }
        bool operator!=(const FastPropertyName& other) const { return m_Hash != other.m_Hash; }
        bool operator!=(const std::string& other) const { return m_Hash != std::hash<std::string>()(other); }
        bool operator!=(const char* other) const { return m_Hash != std::hash<std::string>()(other); }

        uint32_t GetHash() const { return m_Hash; }

    private:
        std::string m_Name;
        uint32_t    m_Hash;
    };
}