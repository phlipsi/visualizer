add_library(engine STATIC
    buffer.h
    buffer.cpp
    destination.h
    destination.cpp
    framebuffer.h
    framebuffer.cpp
    program.h
    program.cpp
    renderbuffer.h
    renderbuffer.cpp
    shader.cpp
    shader.h
    texture.h
    texture.cpp
    vertexarray.h
    vertexarray.cpp
)
target_link_libraries(engine PRIVATE CONAN_PKG::glew CONAN_PKG::glm)
target_include_directories(engine INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(engine PUBLIC cxx_std_17)
