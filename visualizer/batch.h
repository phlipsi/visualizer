#pragma once

#include <GL/glew.h>
#include <glm/mat3x3.hpp>

#include <buffer.h>
#include <vertexarray.h>

#include <vector>

class Batch {
public:
    static const int ATTRIBUTE_POSITION;
    static const int ATTRIBUTE_COLOR;
    static const int ATTRIBUTE_GLOW;

    Batch();

    void add_triangle(const glm::mat3 &vertices, const glm::vec3 &color, float glow);

    void clear();
    void draw() const;
private:
    VertexArray vao;
    Buffer vertex_buffer;

    void add_vertex(const glm::vec3 &vertex, const glm::vec3 &color, float glow);

    std::vector<GLfloat> batch;
};
