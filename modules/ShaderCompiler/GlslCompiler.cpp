#include "GlslCompiler.h"

#include "Core/GlobalContext.h"
#include "Core/Log.h"

namespace wind::glsl
{
    std::vector<uint32_t> CompileGlslToSpv(const std::string& shaderCode, vk::ShaderStageFlagBits stage)
    {
        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;

        options.SetIncluder(std::make_unique<GLSLIncluder>());

        shaderc::SpvCompilationResult result;

        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex:
                result = compiler.CompileGlslToSpv(shaderCode, shaderc_glsl_vertex_shader, "vertex", options);
                break;
            case vk::ShaderStageFlagBits::eFragment:
                result = compiler.CompileGlslToSpv(shaderCode, shaderc_glsl_fragment_shader, "fragment", options);
                break;
            default:
                WIND_CORE_ERROR("Unsupported shader stage.");
                return {};
        }

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            WIND_CORE_ERROR("Failed to compile shader: {0}", result.GetErrorMessage());
            return {};
        }

        return std::vector<uint32_t>(result.cbegin(), result.cend());
    }
} // namespace wind::glsl