cmake_minimum_required(VERSION 3.7)
project(visualizer CXX)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

function(generate_shader_header SHADER)
    get_filename_component(SHADER_NAME "${SHADER}" NAME_WE)
    set(SHADER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.h")
    add_custom_command(OUTPUT "${SHADER_HEADER}"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake" "${SHADER_NAME}" "${SHADER}" "${SHADER_HEADER}"
        DEPENDS "${SHADER}")
    add_custom_target("${SHADER_NAME}" ALL DEPENDS "${SHADER_HEADER}")
endfunction()

generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/shader/vertex.glsl")
generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/shader/fragment.glsl")

add_executable(visualizer visualizer.cpp)
target_link_libraries(visualizer PRIVATE CONAN_PKG::sdl CONAN_PKG::sdl2_gfx)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE CONAN_PKG::sdl CONAN_PKG::glew)
target_include_directories(test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
