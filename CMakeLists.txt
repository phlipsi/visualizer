cmake_minimum_required(VERSION 3.7)
project(visualizer CXX)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

add_executable(visualizer visualizer.cpp)
target_link_libraries(visualizer PRIVATE CONAN_PKG::sdl)
