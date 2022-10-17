#include "vertexarray.h"

#include "buffer.h"

VertexArray::Binding::BufferBinding::BufferBinding(GLenum target, const Buffer &buffer)
  : target(target)
{
    glBindBuffer(target, buffer.get_id());
}

void VertexArray::Binding::BufferBinding::buffer_data(GLsizeiptr size, const void *data, GLenum usage) const {
    glBufferData(target, size, data, usage);
}

VertexArray::Binding::Binding(const VertexArray &vao)
  : bound(true)
{
    glBindVertexArray(vao.id);
}

VertexArray::Binding::~Binding() {
    if (bound) {
        glBindVertexArray(0);
    }
}

void VertexArray::Binding::enable_attribute(GLuint index) const {
    if (bound) {
        glEnableVertexAttribArray(index);
    }
}

void VertexArray::Binding::BufferBinding::vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const {
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

VertexArray::Binding::BufferBinding VertexArray::Binding::bind(GLenum target, const Buffer &buffer) const {
    if (bound) {
        return VertexArray::Binding::BufferBinding(target, buffer);
    }
}

void VertexArray::Binding::unbind(GLenum target) const {
    glBindBuffer(target, 0);
}

VertexArray::VertexArray() {
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

VertexArray::Binding VertexArray::bind() const {
    return Binding(*this);
}
