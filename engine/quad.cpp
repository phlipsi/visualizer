#include "quad.h"

namespace {

GLfloat quad[] = {
    /*   X      Y     S     T    */
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
};

}

const int Quad::ATTRIBUTE_POSITION = 0;
const int Quad::ATTRIBUTE_TEXTURE_COORD = 1;

Quad::Quad() {
    auto binding = vao.bind();
    binding.enable_attribute(ATTRIBUTE_POSITION);
    binding.enable_attribute(ATTRIBUTE_TEXTURE_COORD);

    auto buffer_binding = vertices.bind(GL_ARRAY_BUFFER);
    buffer_binding.vertex_attrib_pointer(binding, ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(0 * sizeof(float)));
    buffer_binding.vertex_attrib_pointer(binding, ATTRIBUTE_TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(2 * sizeof(float)));
    buffer_binding.data(sizeof(quad), quad, GL_STATIC_DRAW);
}

void Quad::draw() const {
    auto binding = vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, sizeof(quad));
}
