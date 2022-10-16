add_library(shader INTERFACE)
target_include_directories(shader INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")

function(generate_shader_header SHADER)
    get_filename_component(SHADER_NAME "${SHADER}" NAME_WE)
    set(SHADER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.h")
    add_custom_command(OUTPUT "${SHADER_HEADER}"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/shader/convert_shader.cmake" "${SHADER_NAME}" "${SHADER}" "${SHADER_HEADER}"
        DEPENDS "${SHADER}")
    target_sources(shader PRIVATE "${SHADER}")
    target_sources(shader PUBLIC "${SHADER_HEADER}")
endfunction()

generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/vertex.glsl")
generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/fragment.glsl")
generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/screen_vertex.glsl")
generate_shader_header("${CMAKE_CURRENT_SOURCE_DIR}/screen_fragment.glsl")
