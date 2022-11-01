#pragma once

#include <framebuffer.h>
#include <texture.h>

namespace visualizer {

class PostProcessing {
public:
    PostProcessing(GLsizei width, GLsizei height);

    Framebuffer::Binding bind_as_target() const;
    Texture::Binding bind_as_source(GLenum texture_unit) const;

    GLint get_texture_id() const { return texture.get_id(); }
private:
    GLsizei width;
    GLsizei height;
    Framebuffer framebuffer;
    Texture texture;
};

}