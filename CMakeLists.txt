cmake_minimum_required(VERSION 3.7)
project(visualizer CXX)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

#add_subdirectory(shader)
add_subdirectory(engine)
add_subdirectory(visualizer)

#add_executable(test
#    test.cpp
#    cube.h
#    cube.cpp
#)
#target_link_libraries(test PRIVATE CONAN_PKG::sdl CONAN_PKG::glew CONAN_PKG::glm shader engine)

add_executable(audio_test audio_test.cpp)
target_link_libraries(audio_test PRIVATE CONAN_PKG::sdl)
