#include "framebuffer.h"

#include "renderbuffer.h"
#include "texture.h"

Framebuffer::Binding::Binding(GLenum target, const Framebuffer &framebuffer)
  : target(target)
{
    glBindFramebuffer(target, framebuffer.id);
}

Framebuffer::Binding::~Binding() {
    if (target) {
        glBindFramebuffer(*target, 0);
    }
}

void Framebuffer::Binding::attach(GLenum attachment, const Texture &texture, GLint level) const {
    if (target) {
        glFramebufferTexture2D(*target, attachment, GL_TEXTURE_2D, texture.get_id(), level);
    }
}

void Framebuffer::Binding::attach(GLenum attachment, const Renderbuffer &renderbuffer) const {
    if (target) {
        glFramebufferRenderbuffer(*target, attachment, GL_RENDERBUFFER, renderbuffer.get_id());
    }
}

GLenum Framebuffer::Binding::get_status() const {
    if (target) {
        return glCheckFramebufferStatus(*target);
    } else {
        return GL_FRAMEBUFFER_UNDEFINED;
    }
}

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &id);
}

Framebuffer::Binding Framebuffer::bind(GLenum target) const {
    return Binding(target, *this);
}
