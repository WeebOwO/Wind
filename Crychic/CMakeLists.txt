cmake_minimum_required(VERSION 3.10)

project(Crychic)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# set include dir
include_directories(Crychic PUBLIC
        ${WIND_INCLUDE_DIR})

include_directories(Crychic PRIVATE
        src/
        extern/taskflow/taskflow
        extern/entt/src/entt)


file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Crychic ${SOURCES})

target_link_libraries(Crychic PRIVATE
        Wind
        glfw
        spdlog::spdlog
)