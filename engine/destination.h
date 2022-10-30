#pragma once

#include "buffer.h"
#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"
#include "vertexarray.h"

#include <stdexcept>

class Destination {
public:
    class Exception : public std::runtime_error {
    public:
        Exception(const char *message) : std::runtime_error(message) { }
    };

    static const int ATTRIBUTE_POSITION;
    static const int ATTRIBUTE_TEXTURE_COORD;

    Destination(GLsizei width, GLsizei height);

    Framebuffer::Binding bind_as_target() const;

    void draw() const;

private:
    GLsizei width;
    GLsizei height;
    Framebuffer framebuffer;
    Texture destination;
    Renderbuffer depth_stencil;
    VertexArray vao;
    Buffer screen_vertices;
};
