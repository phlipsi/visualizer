#include "renderbuffer.h"

Renderbuffer::Binding::Binding(GLenum target, const Renderbuffer &renderbuffer)
  : target(target)
{
    glBindRenderbuffer(target, renderbuffer.id);
}

Renderbuffer::Binding::~Binding() {
    glBindRenderbuffer(target, 0);
}

void Renderbuffer::Binding::storage(GLenum internalformat, GLsizei width, GLsizei height) const {
    glRenderbufferStorage(target, internalformat, width, height);
}

Renderbuffer::Renderbuffer() {
    glGenRenderbuffers(1, &id);
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &id);
}

Renderbuffer::Binding Renderbuffer::bind(GLenum target) const {
    return Binding(target, *this);
}
