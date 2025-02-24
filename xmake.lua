set_project("Wind")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")

includes("xmake/Env.lua")

add_requires("spdlog", "glfw", "vulkansdk", "entt", "vk-bootstrap", "vulkan-memory-allocator", "glm", "shaderc", "vulkansdk", "spirv-cross", "nlohmann_json")
set_policy("package.requires_lock", true)

set_runtimes("MD")

add_linkdirs("external/slang/lib/")

target("Renderer")
    set_kind("binary")
    add_options("env")
    set_languages("c++20")
    add_includedirs("modules/", "xmake/", "external/slang/include/", "external/meshoptimizer/")
    add_headerfiles("modules/**.h")
    add_files("modules/**.cpp", "external/meshoptimizer/*.cpp")
    add_packages("spdlog", "glfw", "entt", "vk-bootstrap", "vulkansdk", "vulkan-memory-allocator", "glm", "shaderc", "spirv-cross", "nlohmann_json")
    add_links("slang")
    add_configfiles("xmake/Config.h.in")
    set_configdir("xmake/")

    if is_mode("debug") then
        add_defines("ENABLE_JOB_TEST")
    end
    -- post build step to copy dlls
    after_build(function(target)
        os.cp("external/slang/bin/slang.dll", target:targetdir())
    end)