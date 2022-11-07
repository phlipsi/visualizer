#pragma once

#include "object.h"

#include <memory>
#include <vector>

namespace visualizer {

class Collection : public Object {
public:
    explicit Collection(std::initializer_list<std::shared_ptr<Object>> objects);

    void draw(Batch &batch, const glm::mat4 &model) const override;

private:
    std::vector<std::shared_ptr<Object>> objects;
};

}