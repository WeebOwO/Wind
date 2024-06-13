cmake_minimum_required(VERSION 3.10)

project(Wind)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include_directories(Wind public
        ${Vulkan_INCLUDE_DIRS}
        "include/")

file(GLOB_RECURSE WIND_SOURCES "src/*.cpp")

add_subdirectory(extern/vk-bootstrap)
add_subdirectory(extern/vma)

add_library(Wind STATIC ${WIND_SOURCES}
        include/Buffer.h
        src/Buffer.cpp)

target_link_libraries(Wind PUBLIC
        ${Vulkan_LIBRARIES}
        vk-bootstrap::vk-bootstrap
        glfw
        GPUOpen::VulkanMemoryAllocator
)


