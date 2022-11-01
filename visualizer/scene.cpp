#include "scene.h"

#include <stdexcept>

namespace visualizer {

Scene::Scene(GLsizei width, GLsizei height)
    : width(width),
      height(height)
{
    {
        auto binding = color.bind(GL_TEXTURE0, GL_TEXTURE_2D);
        binding.image_2d(0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        binding.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        binding.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    {
        auto binding = glow.bind(GL_TEXTURE0, GL_TEXTURE_2D);
        binding.image_2d(0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        binding.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        binding.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    {
        auto binding = depth.bind(GL_RENDERBUFFER);
        binding.storage(GL_DEPTH_COMPONENT, width, height);
    }
    {
        auto binding = framebuffer.bind(GL_FRAMEBUFFER);
        binding.attach(GL_COLOR_ATTACHMENT0, color, 0);
        binding.attach(GL_COLOR_ATTACHMENT1, glow, 0);
        binding.attach(GL_DEPTH_ATTACHMENT, depth);
        const GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (binding.get_status() != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Scene framebuffer not complete");
        }
    }
}

Framebuffer::Binding Scene::bind_as_target() const {
    glViewport(0, 0, width, height);
    return framebuffer.bind(GL_FRAMEBUFFER);
}

Texture::Binding Scene::bind_colors_as_source(GLenum texture_unit) const {
    return color.bind(texture_unit, GL_TEXTURE_2D);
}

Texture::Binding Scene::bind_glow_as_source(GLenum texture_unit) const {
    return glow.bind(texture_unit, GL_TEXTURE_2D);
}

}
