cmake_minimum_required(VERSION 3.10)

project(Crychic)

set(CMAKE_CXX_STANDARD 20)
set(DILIGENT_INSTALL_TOOLS ON)

add_subdirectory(extern/DiligentCore)
add_subdirectory(extern/DiligentTools)

set(BUILD_Version 1)
set(MONO_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/mono/libs")
set(CRYCHIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

add_library(mono-2.0-sgen STATIC IMPORTED)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_LOCATION "${MONO_LIB_DIR}/Debug/mono-2.0-sgen.lib")
else()
    set_target_properties(mono-2.0-sgen PROPERTIES IMPORTED_LOCATION "${MONO_LIB_DIR}/Release/mono-2.0-sgen.lib")
endif()

cmake_path(SET SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
cmake_path(SET MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Config.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(Crychic
        ${SOURCES}
        extern/DiligentTools/ThirdParty/imgui/imgui_demo.cpp
)

target_include_directories(Crychic PRIVATE
        extern/DiligentTools/TextureLoader/interface
        extern/DiligentTools/Imgui/interface/
        extern/DiligentTools/ThirdParty/imgui/
        extern/glm/
        extern/mono/include/
        include/
        src/
        extern/entt/src/entt
)

target_precompile_headers(Crychic PRIVATE include/RenderCore.h)

target_compile_options(Crychic PRIVATE -DUNICODE -DENGINE_DLL)

target_link_libraries(Crychic PRIVATE
        Diligent-GraphicsEngineD3D12-shared
        Diligent-Common
        Diligent-TextureLoader
        Diligent-Imgui
        mono-2.0-sgen
)

# copy_required_dlls is assumed to be defined elsewhere
copy_required_dlls(Crychic)
