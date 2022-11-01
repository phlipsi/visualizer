#pragma once

#include <buffer.h>
#include <framebuffer.h>
#include <renderbuffer.h>
#include <texture.h>
#include <vertexarray.h>

namespace visualizer {

class Scene {
public:
    Scene(GLsizei width, GLsizei height);

    Framebuffer::Binding bind_as_target() const;

    Texture::Binding bind_colors_as_source(GLenum texture_unit) const;
    Texture::Binding bind_glow_as_source(GLenum texture_unit) const;

    GLint get_color_buffer_id() const { return color.get_id(); }
private:
    GLsizei width;
    GLsizei height;
    Framebuffer framebuffer;
    Texture color;
    Texture glow;
    Renderbuffer depth;
};

}