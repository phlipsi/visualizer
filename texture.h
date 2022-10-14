#pragma once

#include <GL/glew.h>

#include <optional>

class Texture {
public:
    class Binding {
    public:
        Binding(GLenum target, const Texture &texture);
        ~Binding();

        Binding(Binding &&);

        void image_2d(GLint level, GLint internalformat, GLsizei width,
                      GLsizei height, GLint border, GLenum format,
                      GLenum type, const void *data) const;
        void set_parameter(GLenum pname, GLint param) const;
        void generate_mipmap() const;

        Binding(const Binding &) = delete;
        Binding &operator = (const Binding &) = delete;
        Binding &operator = (Binding &&) = delete;
    private:
        std::optional<GLenum> target;
    };

    Texture();
    ~Texture();

    Binding bind(GLenum target);

    Texture(const Texture &) = delete;
    Texture &operator = (const Texture &) = delete;
private:
    GLuint id;
};
