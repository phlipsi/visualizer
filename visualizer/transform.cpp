#include "transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace visualizer {

Transform::Transform(std::shared_ptr<Object> object)
  : object(object)
{ }

void Transform::draw(Batch &batch, const glm::mat4& model) const {
    object->draw(batch, get_transform(model));
}

Scale::Scale(std::shared_ptr<Object> object, const float &factor)
  : Transform(object), factor(&factor)
{ }

glm::mat4 Scale::get_transform(const glm::mat4 &model) const {
    return glm::scale(model, glm::vec3(*factor, *factor, 1.0f));
}

Rotate::Rotate(std::shared_ptr<Object> object, const float &angle)
  : Transform(object),
    angle(&angle)
{ }

glm::mat4 Rotate::get_transform(const glm::mat4 &model) const {
    return glm::rotate(model, *angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

Translate::Translate(std::shared_ptr<Object> object, const glm::vec3 &direction)
  : Transform(object),
    direction(&direction)
{ }

glm::mat4 Translate::get_transform(const glm::mat4 &model) const {
    return glm::translate(model, *direction);
}

}
