cmake_minimum_required(VERSION 3.7)
project(visualizer CXX)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

add_subdirectory(shader)

add_executable(visualizer visualizer.cpp)
target_link_libraries(visualizer PRIVATE CONAN_PKG::sdl CONAN_PKG::sdl2_gfx)

add_executable(test test.cpp shader.cpp shader.h program.h program.cpp)
target_link_libraries(test PRIVATE CONAN_PKG::sdl CONAN_PKG::glew shader)
