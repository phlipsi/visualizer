#include "shape.h"

#include "batch.h"

#include <glm/gtc/matrix_transform.hpp>

namespace visualizer {

Shape::Shape(const glm::vec3 &color, const float &glow)
    : Shape(color, glow, {}) { }

Shape::Shape(const glm::vec3 &color, const float &glow, std::initializer_list<glm::mat4> triangles)
  : color(color), glow(&glow), triangles(triangles)
{ }

void Shape::draw(Batch &batch, const glm::mat4 &model) const {
    for (const auto &x : triangles) {
        batch.add_triangle(model * x, color, *glow);
    }
}

void Shape::add_triangle(const glm::mat4 &t) {
    triangles.push_back(t);
}

namespace {

const glm::mat4 triangle(
    -0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
     0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
     0.0f,  sqrtf(3.0f) / 3.0f, 0.0f, 1.0f,
     0.0f,  0.0f,               0.0f, 1.0f);

}

Triangle::Triangle(const glm::vec3 &color, const float &glow)
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

Rectangle::Rectangle(const glm::vec3 &color, const float &glow)
  : Shape(color, glow, { rectangle_upper_left, rectangle_lower_right })
{ }

Circle::Circle(const glm::vec3 &color, const float &glow)
    : Shape(color, glow)
{
    const int num = 16;
    for (int i = 0; i < num; ++i) {
        const float alpha = glm::radians(i * 360.0f / num);
        const float beta  = glm::radians((i + 1) * 360.0f / num);
        const glm::mat4 t(
            0.0f, 0.0f, 0.0f, 1.0f,
            0.5f * std::cos(alpha), 0.5f * std::sin(alpha), 0.0f, 1.0f,
            0.5f * std::cos(beta), 0.5f * std::sin(beta), 0.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        add_triangle(t);
    }
}

}
