cmake_minimum_required(VERSION 3.10)

project(Sandbox)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

include_directories(
        ${CRYCHIC_INCLUDE_DIR}
        include/
)

add_executable(Sandbox WIN32
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(Sandbox
        PRIVATE
        Crychic
)

add_dependencies(Sandbox Crychic)

function(copy_script_dll target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:ScriptRuntime>
            $<TARGET_FILE_DIR:${target}>)
endfunction()

# copy_required_dlls is assumed to be defined else
copy_required_dlls(Sandbox)
