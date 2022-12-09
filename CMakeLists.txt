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

add_library(imgui_impl STATIC ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp ${CMAKE_BINARY_DIR}/bindings/imgui_impl_sdl.cpp)
target_include_directories(imgui_impl PUBLIC ${CMAKE_BINARY_DIR}/bindings)
target_link_libraries(imgui_impl PUBLIC CONAN_PKG::sdl CONAN_PKG::imgui)
