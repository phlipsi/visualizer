#pragma once

#include <GL/glew.h>

class Buffer {
private:
    class Binding {
    public:
        Binding(GLenum target, const Buffer &buffer);
        ~Binding();

        Binding(Binding &&) = delete;
        Binding(const Binding &) = delete;
        Binding &operator = (const Binding &) = delete;
        Binding &operator = (Binding &&) = delete;
    protected:
        std::optional<GLenum> get_target() const { return target; }
    private:
        std::optional<GLenum> target;
    };

public:
    class ArrayBufferBinding : public Binding {
    public:
        ArrayBufferBinding(const Buffer &buffer);
        ~ArrayBufferBinding();

        void vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const;
        void buffer_data(GL

        ArrayBufferBinding(ArrayBufferBinding &&) = delete;
        ArrayBufferBinding(const ArrayBufferBinding &) = delete;
        ArrayBufferBinding &operator = (const ArrayBufferBinding &) = delete;
        ArrayBufferBinding &operator = (ArrayBufferBinding &&) = delete;
    private:
        std::optional<GLenum> target;
    };

    Buffer();
    ~Buffer();

    ArrayBufferBinding array_buffer_bind() const;

    Buffer(const Buffer &) = delete;
    Buffer &operator = (const Buffer &) = delete;
private:
    GLuint id;
};
