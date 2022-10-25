#include "buffer.h"

Buffer::Binding::Binding(GLenum target, const Buffer &buffer)
  : target(target)
{
    glBindBuffer(target, buffer.get_id());
}

Buffer::Binding::~Binding() {
    glBindBuffer(target, 0);
}

void Buffer::Binding::data(GLsizeiptr size, const void *data, GLenum usage) const {
    glBufferData(target, size, data, usage);
}

void Buffer::Binding::subdata(GLintptr offset, GLsizeiptr size, const void *data) const {
    glBufferSubData(target, offset, size, data);
}

void Buffer::Binding::vertex_attrib_pointer(const VertexArray::Binding &vao, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const {
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

Buffer::Buffer() {
    glGenBuffers(1, &id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}

Buffer::Binding Buffer::bind(GLenum target) const {
    return Binding(target, *this);
}
