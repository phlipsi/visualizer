#include "ring.h"

#include <glm/gtc/matrix_transform.hpp>

namespace visualizer {

Ring::Ring(unsigned int num_repetitions, float radius,
           std::initializer_list<std::shared_ptr<Object>> objects)
  : num_repetitions(num_repetitions),
    radius(radius),
    objects(objects)
{ }

void Ring::draw(Batch &batch, const glm::mat4 &model) const {
    const auto num_objects = objects.size();
    const float angle = 360.0f / (num_repetitions * num_objects);
    for (unsigned int i = 0; i < num_repetitions; ++i) {
        for (int j = 0; j < num_objects; j++) {
            glm::mat4 rotation = model;
            rotation = glm::rotate(rotation, glm::radians(angle * (j + num_objects * i)), glm::vec3(0.0f, 0.0f, 1.0f));
            rotation = glm::translate(rotation, glm::vec3(0.0f, radius, 0.0f));
            objects[j]->draw(batch, rotation);
        }
    }
}

}
