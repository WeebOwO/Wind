#pragma once

#include "Backend/Shader.h"

namespace wind
{
    template <typename T>
    class ShaderParameter
    {
    public:
        ShaderParameter(const std::string& name, const T& value)
            : m_Name(name), m_Value(value) 
            {
                m_NameHash = std::hash<std::string>{}(m_Name);
            }

        void SetValue(const T& value) 
        {
            
        }

    private:
        std::string m_Name;
        int         m_NameHash;
        T           m_Value;
    };
} // namespace wind