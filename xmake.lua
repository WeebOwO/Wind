set_project("Wind")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")

includes("external/slang/")

add_requires("spdlog", "glfw", "vulkansdk", "entt", "vk-bootstrap", "rttr", "vulkan-memory-allocator", "glm")
set_policy("package.requires_lock", true)

target("Renderer")
    set_kind("binary")
    add_deps("slang")
    set_languages("c++20")
    add_includedirs("include/")
    add_headerfiles("include/**.h")
    add_files("src/**.cpp")
    add_packages("spdlog", "glfw", "vulkansdk", "entt", "vk-bootstrap", "rttr", "vulkan-memory-allocator", "glm")
    
    if is_mode("debug") then
        add_defines("ENABLE_JOB_TEST")
    end

