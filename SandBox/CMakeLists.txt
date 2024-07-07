cmake_minimum_required(VERSION 3.10)

project(Sandbox)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(Sandbox PRIVATE
        ../Crychic/include
        include/
)

add_executable(Sandbox WIN32 ${SOURCES})

target_link_libraries(Sandbox PRIVATE Crychic)

copy_required_dlls(Sandbox)