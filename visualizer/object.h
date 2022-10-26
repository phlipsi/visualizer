#pragma once

#include <glm/mat4x4.hpp>

namespace visualizer {

class Batch;

class Object {
public:
    virtual ~Object() = default;
    virtual void draw(Batch &batch, const glm::mat4 &model) const = 0;
};

}
