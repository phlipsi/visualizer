add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "scene_vertex_shader" "${CMAKE_CURRENT_SOURCE_DIR}/scene.vert" "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    DEPENDS "scene.vert")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "scene_fragment_shader" "${CMAKE_CURRENT_SOURCE_DIR}/scene.frag" "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    DEPENDS "scene.frag")

add_executable(visualizer
    batch.h
    batch.cpp
    object.h
    ring.h
    ring.cpp
    shape.h
    shape.cpp
    transform.h
    transform.cpp
    visualizer.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene.frag"
    "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene.vert"
)
target_link_libraries(visualizer PRIVATE CONAN_PKG::sdl CONAN_PKG::glew CONAN_PKG::glm engine)
target_include_directories(visualizer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
