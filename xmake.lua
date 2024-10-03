set_project("Wind")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")

includes("external/slang/")
includes("xmake/Env.lua")

add_requires("spdlog", "glfw", "vulkansdk", "entt", "vk-bootstrap", "vulkan-memory-allocator", "glm", "shaderc", "vulkansdk", "spirv-cross")
set_policy("package.requires_lock", true)

set_runtimes("MD")

target("Renderer")
    set_kind("binary")
    add_options("env")
    add_deps("slang")
    set_languages("c++20")
    add_includedirs("include/", "xmake/")
    add_headerfiles("include/**.h")
    add_files("src/**.cpp")
    add_packages("spdlog", "glfw", "vulkansdk", "entt", "vk-bootstrap", "vulkan-memory-allocator", "glm", "shaderc", "spirv-cross")

    add_configfiles("xmake/Config.h.in")
    set_configdir("xmake/")

    if is_mode("debug") then
        add_defines("ENABLE_JOB_TEST")
    end