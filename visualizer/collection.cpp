#include "collection.h"

namespace visualizer {

Collection::Collection(std::initializer_list<std::shared_ptr<Object>> objects)
    : objects(objects) { }

void Collection::draw(Batch &batch, const glm::mat4 &model) const {
    for (const auto &item : objects) {
        item->draw(batch, model);
    }
}

}
