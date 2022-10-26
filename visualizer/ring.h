#pragma once

#include "object.h"

#include <memory>
#include <vector>

namespace visualizer {

class Ring : public Object {
public:
    Ring(unsigned int num_repetitions, float radius,
         std::initializer_list<std::shared_ptr<Object>> objects);

    void draw(Batch &batch, const glm::mat4 &model) const override;

private:
    unsigned int num_repetitions;
    float radius;
    std::vector<std::shared_ptr<Object>> objects;
};

}
