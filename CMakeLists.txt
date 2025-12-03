cmake_minimum_required(VERSION 3.10)
project(osm_cpp_demo LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(osm_cpp_demo
    src/main.cpp
    src/collectors/default_collector/default_collector.cpp
)

target_include_directories(osm_cpp_demo PRIVATE
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/external/libosmium/include
)

target_link_libraries(osm_cpp_demo PRIVATE ZLIB::ZLIB pthread)
