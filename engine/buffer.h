#pragma once

#include "vertexarray.h"

#include <GL/glew.h>

#include <optional>

class Buffer {
public:
    class Binding {
    public:
        Binding(GLenum target, const Buffer &buffer);
        ~Binding();

        void data(GLsizeiptr size, const void *data, GLenum usage) const;
        void subdata(GLintptr offset, GLsizeiptr size, const void *data) const;
        void vertex_attrib_pointer(const VertexArray::Binding &vao, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const;
    private:
        GLenum target;

    };

    Buffer();
    ~Buffer();

    Binding bind(GLenum target) const;
    GLuint get_id() const { return id; }

    Buffer(const Buffer &) = delete;
    Buffer &operator = (const Buffer &) = delete;
private:
    GLuint id;
};
