cmake_minimum_required(VERSION 3.10)

project(Crychic)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(extern/DiligentCore)

# set include dir
include_directories(Crychic PRIVATE
        src/
        extern/taskflow/taskflow
        extern/entt/src/entt)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Crychic ${SOURCES})

target_compile_options(Crychic PRIVATE -DUNICODE -DENGINE_DLL)

target_link_libraries(Crychic PRIVATE
        glfw
        spdlog::spdlog
        Diligent-GraphicsEngineD3D11-shared
        Diligent-GraphicsEngineOpenGL-shared
        Diligent-GraphicsEngineD3D12-shared
        Diligent-GraphicsEngineVk-shared
        Diligent-Common
)

copy_required_dlls(Crychic)