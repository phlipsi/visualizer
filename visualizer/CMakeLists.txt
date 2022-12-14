add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "scene_vertex_shader" "${CMAKE_CURRENT_SOURCE_DIR}/scene.vert" "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    DEPENDS "scene.vert")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "scene_fragment_shader" "${CMAKE_CURRENT_SOURCE_DIR}/scene.frag" "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    DEPENDS "scene.frag")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/screen.vert.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "screen_vertex_shader" "${CMAKE_CURRENT_SOURCE_DIR}/screen.vert" "${CMAKE_CURRENT_BINARY_DIR}/screen.vert.h"
    DEPENDS "screen.vert")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/screen.frag.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "screen_fragment_shader" "${CMAKE_CURRENT_SOURCE_DIR}/screen.frag" "${CMAKE_CURRENT_BINARY_DIR}/screen.frag.h"
    DEPENDS "screen.frag")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/blur.vert.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "blur_vertex_shader" "${CMAKE_CURRENT_SOURCE_DIR}/blur.vert" "${CMAKE_CURRENT_BINARY_DIR}/blur.vert.h"
    DEPENDS "blur.vert")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/blur.frag.h"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake"
        "blur_fragment_shader" "${CMAKE_CURRENT_SOURCE_DIR}/blur.frag" "${CMAKE_CURRENT_BINARY_DIR}/blur.frag.h"
    DEPENDS "blur.frag")

add_executable(visualizer
    action.h
    action.cpp
    batch.h
    batch.cpp
    collection.h
    collection.cpp
    object.h
    parameter.h
    parameter.cpp
    parameters.h
    parameters.cpp
    postprocessing.h
    postprocessing.cpp
    ring.h
    ring.cpp
    scene.h
    scene.cpp
    shape.h
    shape.cpp
    transform.h
    transform.cpp
    visualizer.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/scene.frag.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene.frag"
    "${CMAKE_CURRENT_BINARY_DIR}/scene.vert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/scene.vert"
    "${CMAKE_CURRENT_BINARY_DIR}/screen.frag.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/screen.frag"
    "${CMAKE_CURRENT_BINARY_DIR}/screen.vert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/screen.vert"
    "${CMAKE_CURRENT_BINARY_DIR}/blur.frag.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/blur.frag"
    "${CMAKE_CURRENT_BINARY_DIR}/blur.vert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/blur.vert"
    "${CMAKE_SOURCE_DIR}/choreography.json"
    "${CMAKE_SOURCE_DIR}/plot.py"
)
target_link_libraries(visualizer PRIVATE
    CONAN_PKG::sdl
    CONAN_PKG::glew
    CONAN_PKG::glm
    CONAN_PKG::nlohmann_json
    imgui_impl
    CONAN_PKG::implot
    engine)
target_include_directories(visualizer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
