#include "texture.h"

Texture::Binding::Binding(GLenum target, const Texture &texture)
  : target(target)
{
    glBindTexture(target, texture.id);
}

Texture::Binding::~Binding() {
    if (target) {
        glBindTexture(*target, 0);
    }
}

void Texture::Binding::image_2d(GLint level, GLint internalformat,
                                GLsizei width, GLsizei height, GLint border,
                                GLenum format, GLenum type,
                                const void *data) const
{
    if (target) {
        glTexImage2D(*target, level, internalformat, width, height, border,
                     format, type, data);
    }
}

void Texture::Binding::set_parameter(GLenum pname, GLint param) const {
    if (target) {
        glTexParameteri(*target, pname, param);
    }
}

void Texture::Binding::generate_mipmap() const {
    if (target) {
        glGenerateMipmap(*target);
    }
}

Texture::Texture() {
    glGenTextures(1, &id);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

Texture::Binding Texture::bind(GLenum target) const {
    return Binding(target, *this);
}
