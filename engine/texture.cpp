#include "texture.h"

Texture::Binding::Binding(GLenum texture_unit, GLenum target, const Texture &texture)
  : texture_unit(texture_unit), target(target)
{
    glActiveTexture(texture_unit);
    glBindTexture(target, texture.id);
}

Texture::Binding::~Binding() {
    glActiveTexture(texture_unit);
    glBindTexture(target, 0);
}

void Texture::Binding::image_2d(GLint level, GLint internalformat,
                                GLsizei width, GLsizei height, GLint border,
                                GLenum format, GLenum type,
                                const void *data) const
{
    glTexImage2D(target, level, internalformat, width, height, border,
                    format, type, data);
}

void Texture::Binding::set_parameter(GLenum pname, GLint param) const {
    glTexParameteri(target, pname, param);
}

void Texture::Binding::generate_mipmap() const {
    glGenerateMipmap(target);
}

Texture::Texture() {
    glGenTextures(1, &id);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

Texture::Binding Texture::bind(GLenum texture_unit, GLenum target) const {
    return Binding(texture_unit, target, *this);
}
