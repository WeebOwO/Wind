#pragma once

#include <fstream>
#include <shaderc/shaderc.hpp>

#include "Backend/Guard.h"
#include "Core/GlobalContext.h"

namespace wind::glsl
{
    struct GLSLIncluder : public shaderc::CompileOptions::IncluderInterface
    {
        shaderc_include_result* GetInclude(const char*          requested_source,
                                           shaderc_include_type type,
                                           const char*          requesting_source,
                                           size_t               include_depth) override
        {
            auto shaderRootDir = g_GlobalContext->pathManager.GetShaderRootDir();
            auto filePath = shaderRootDir / requested_source;
            std::string content = Read(filePath.string().c_str());

            shaderc_include_result* result = new shaderc_include_result();
            result->content                = content.c_str();
            result->source_name            = requested_source;
            result->source_name_length     = strlen(requested_source);
            result->content_length         = content.size();
            return result;
        }

        void ReleaseInclude(shaderc_include_result* include_result) override 
        {
            delete include_result;
        }

    private:
        static std::string Read(const char* filename)
        {
            // read file as text
            std::ifstream file(filename);
            std::string   content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return content;
        }
    };

    std::vector<uint32_t> CompileGlslToSpv(const std::string& shaderCode, vk::ShaderStageFlagBits stage);
}