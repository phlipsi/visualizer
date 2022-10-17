#include "vertexarray.h"

#include "buffer.h"

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

VertexArray::VertexArray() {
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

VertexArray::Binding VertexArray::bind() const {
    return Binding(*this);
}
