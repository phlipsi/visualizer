#include "shape.h"

#include "batch.h"

namespace visualizer {

Shape::Shape(const glm::vec3 &color, float glow, std::initializer_list<glm::mat4> triangles)
  : color(color), glow(glow), triangles(triangles)
{ }

void Shape::draw(Batch &batch, const glm::mat4 &model) const {
    for (const auto &x : triangles) {
        batch.add_triangle(model * x, color, glow);
    }
}

namespace {

const glm::mat4 triangle(
    -0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
     0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
     0.0f,  sqrtf(3.0f) / 3.0f, 0.0f, 1.0f,
     0.0f,  0.0f,               0.0f, 1.0f);

}

Triangle::Triangle(const glm::vec3 &color, float glow)
  : Shape(color, glow, {triangle}) { }

namespace {

const glm::mat4 rectangle_upper_left(
    -0.5f, -0.5f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f,
     0.0f,  0.0f, 0.0f, 1.0f
);

const glm::mat4 rectangle_lower_right(
    -0.5f, -0.5f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 1.0f,
     0.0f, 0.0f, 0.0f, 1.0f
);

}

Rectangle::Rectangle(const glm::vec3 &color, float glow)
  : Shape(color, glow, { rectangle_upper_left, rectangle_lower_right })
{ }

}
