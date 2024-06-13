cmake_minimum_required(VERSION 3.10)

project(WindEngine)

set(WIND_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Wind/include")

add_subdirectory("Wind")
add_subdirectory("Crychic")
