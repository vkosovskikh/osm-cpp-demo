cmake_minimum_required(VERSION 3.10)
project(osm_cpp_demo LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(osm_cpp_demo
    src/main.cpp
)

target_include_directories(osm_cpp_demo PRIVATE
    external/libosmium/include
)

add_executable(test_boundary_handler
    src/test/test_boundary_handler.cpp
    src/relations/boundary/RelationBoundaryHandler.cpp
)

target_link_libraries(test_boundary_handler PRIVATE ZLIB::ZLIB pthread)

target_include_directories(test_boundary_handler PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/libosmium/include
)
