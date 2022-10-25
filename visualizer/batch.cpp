#include "batch.h"

#include <glm/gtc/type_ptr.hpp>

namespace {

const std::vector<GLfloat>::size_type CAPACITY = 10000 * 7;

}

const int Batch::ATTRIBUTE_POSITION = 0;
const int Batch::ATTRIBUTE_COLOR = 1;
const int Batch::ATTRIBUTE_GLOW = 2;

Batch::Batch() {
    auto binding = vao.bind();
    binding.enable_attribute(ATTRIBUTE_POSITION);
    binding.enable_attribute(ATTRIBUTE_COLOR);
    binding.enable_attribute(ATTRIBUTE_GLOW);

    auto buffer_binding = vertex_buffer.bind(GL_ARRAY_BUFFER);
    buffer_binding.vertex_attrib_pointer(binding, ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void *)(0 * sizeof(GLfloat)));
    buffer_binding.vertex_attrib_pointer(binding, ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void *)(3 * sizeof(GLfloat)));
    buffer_binding.vertex_attrib_pointer(binding, ATTRIBUTE_GLOW, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void *)(6 * sizeof(GLfloat)));
    buffer_binding.data(CAPACITY * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

    batch.reserve(CAPACITY);
}

void Batch::add_vertex(const glm::vec3 &vertex, const glm::vec3 &color, float glow) {
    if (batch.size() == CAPACITY) {
        throw std::runtime_error("Maximal number of batched vertices already reached");
    }
    batch.push_back(vertex.x);
    batch.push_back(vertex.y);
    batch.push_back(vertex.z);
    batch.push_back(color.r);
    batch.push_back(color.g);
    batch.push_back(color.b);
    batch.push_back(glow);
}


void Batch::add_triangle(const glm::mat3 &vertices, const glm::vec3 &color, float glow) {
    add_vertex(vertices[0], color, glow);
    add_vertex(vertices[1], color, glow);
    add_vertex(vertices[2], color, glow);
}

void Batch::clear() {
    batch.clear();
}

void Batch::draw() const {
    auto binding = vao.bind();
    auto buffer_binding = vertex_buffer.bind(GL_ARRAY_BUFFER);
    buffer_binding.subdata(0, batch.size() * sizeof(GLfloat), batch.data());
    //void *data = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    //glUnmapBuffer(GL_ARRAY_BUFFER);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batch.size()));
}
