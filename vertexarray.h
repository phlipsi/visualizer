#pragma once

#include <GL/glew.h>

#include <optional>

class Buffer;

class VertexArray {
public:
    class Binding {
    public:
        class BufferBinding {
        public:
            BufferBinding(GLenum target, const Buffer &buffer);

            void buffer_data(GLsizeiptr size, const void *data, GLenum usage) const;
            void vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const;
        private:
            GLenum target;
        };

        Binding(const VertexArray &vao);
        ~Binding();

        void enable_attribute(GLuint index) const;
        BufferBinding bind(GLenum target, const Buffer &buffer) const;
        void unbind(GLenum target) const;

        Binding(Binding &&) = delete;
        Binding(const Binding &) = delete;
        Binding &operator = (const Binding &) = delete;
        Binding &operator = (Binding &&) = delete;
    private:
        bool bound;
    };

    VertexArray();
    ~VertexArray();

    Binding bind();

    VertexArray(const VertexArray &) = delete;
    VertexArray &operator = (const VertexArray &) = delete;
private:
    GLuint id;
};
