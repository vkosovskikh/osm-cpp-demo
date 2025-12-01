cmake_minimum_required(VERSION 3.10)
project(osm_cpp_demo LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)

add_executable(osm_cpp_demo
    src/main.cpp
)

target_include_directories(osm_cpp_demo PRIVATE
    external/libosmium/include
)
