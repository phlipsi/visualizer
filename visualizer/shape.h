#pragma once

#include "object.h"

#include <glm/vec3.hpp>

#include <vector>

namespace visualizer {

class Shape : public Object {
public:
    Shape(const glm::vec3 &color, const float &glow);
    Shape(const glm::vec3 &color, const float &glow, std::initializer_list<glm::mat4> triangles);

    void draw(Batch &batch, const glm::mat4 &model) const override;

    void add_triangle(const glm::mat4 &t);
private:
    glm::vec3 color;
    const float *glow;
    std::vector<glm::mat4> triangles;
};

class Triangle : public Shape {
public:
    Triangle(const glm::vec3 &color, const float &glow);
};

class Rectangle : public Shape {
public:
    Rectangle(const glm::vec3 &color, const float &glow);
};

class Circle : public Shape {
public:
    Circle(const glm::vec3 &color, const float &glow);
};

}
