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

Deform::Deform(std::shared_ptr<Object> object, const float &width, const float &height)
  : Transform(object),
    width(&width),
    height(&height)
{ }

glm::mat4 Deform::get_transform(const glm::mat4& model) const {
    return glm::scale(model, glm::vec3(*width, *height, 1.0f));
}

Rotate::Rotate(std::shared_ptr<Object> object, const float &degree)
  : Transform(object),
    degree(&degree)
{ }

glm::mat4 Rotate::get_transform(const glm::mat4 &model) const {
    return glm::rotate(model, glm::radians(*degree), glm::vec3(0.0f, 0.0f, 1.0f));
}

Translate::Translate(std::shared_ptr<Object> object, const glm::vec3 &direction)
  : Transform(object),
    x(direction.x),
    y(direction.y),
    z(direction.z),
    rz(nullptr)
{ }

Translate::Translate(std::shared_ptr<Object> object, float x, float y, const float &z)
  : Transform(object),
    x(x),
    y(y),
    z(0.0),
    rz(&z)
{ }

glm::mat4 Translate::get_transform(const glm::mat4 &model) const {
    glm::vec3 direction(x, y, rz != nullptr ? *rz : z);
    return glm::translate(model, direction);
}

}
