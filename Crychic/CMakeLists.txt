cmake_minimum_required(VERSION 3.10)

project(Crychic)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(extern/DiligentCore)
add_subdirectory(extern/DiligentTools)

# set include dir
include_directories(Crychic PRIVATE
        extern/DiligentTools/TextureLoader/interface
        include/
        src/
        extern/taskflow/taskflow
        extern/entt/src/entt)

set(BUILD_Version 1)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Config.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h
)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(Crychic ${SOURCES})

target_compile_options(Crychic PRIVATE -DUNICODE -DENGINE_DLL)

target_link_libraries(Crychic PRIVATE
        glfw
        spdlog::spdlog
        Diligent-GraphicsEngineD3D11-shared
        Diligent-GraphicsEngineOpenGL-shared
        Diligent-GraphicsEngineD3D12-shared
        Diligent-GraphicsEngineVk-shared
        Diligent-Common
        Diligent-TextureLoader
)

copy_required_dlls(Crychic)