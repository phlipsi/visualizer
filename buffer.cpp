#include "buffer.h"

Buffer::Binding::Binding(GLenum target, const Buffer &buffer)
  : target(target)
{
    glBindBuffer(target, buffer.id);
}

Buffer::Binding::~Binding() {
    glBindBuffer

Buffer::Buffer() {
    glGenBuffers(1, &id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}

Buffer::Buffer::bind(GLenum target) const {
    glBindBuffer(
    return Binding(target, *this);
}

void Buffer::vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const {
    glVertexAttribPointer(index, size, 
