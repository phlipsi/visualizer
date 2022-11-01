#include "postprocessing.h"

#include <stdexcept>

namespace visualizer {

PostProcessing::PostProcessing(GLsizei width, GLsizei height)
    : width(width), height(height) 
{
    {
        auto binding = texture.bind(GL_TEXTURE0, GL_TEXTURE_2D);
        binding.image_2d(0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        binding.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        binding.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        binding.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    {
        auto binding = framebuffer.bind(GL_FRAMEBUFFER);
        binding.attach(GL_COLOR_ATTACHMENT0, texture, 0);

        if (binding.get_status() != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Post-processing framebuffer not complete");
        }
    }
}

Framebuffer::Binding PostProcessing::bind_as_target() const {
    glViewport(0, 0, width, height);
    return framebuffer.bind(GL_FRAMEBUFFER);
}

Texture::Binding PostProcessing::bind_as_source(GLenum texture_unit) const {
    return texture.bind(texture_unit, GL_TEXTURE_2D);
}


}
