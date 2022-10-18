#pragma once

#include <GL/glew.h>

#include <optional>

class Renderbuffer;
class Texture;

class Framebuffer {
public:
    class Binding {
    public:
        ~Binding();

        void attach(GLenum attachment, const Texture &texture, GLint level) const;
        void attach(GLenum attachment, const Renderbuffer &renderbuffer) const;

        GLenum get_status() const;

        Binding(Binding &&) = delete;
        Binding(const Binding &) = delete;
        Binding &operator = (const Binding &) = delete;
        Binding &operator = (Binding &&) = delete;
    private:
        friend class Framebuffer;
        Binding(GLenum target, const Framebuffer &framebuffer);
        std::optional<GLenum> target;
    };
    Framebuffer();
    ~Framebuffer();

    Binding bind(GLenum target) const;

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator = (const Framebuffer &) = delete;
private:
    GLuint id;
};
