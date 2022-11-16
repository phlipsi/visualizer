#include "object.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace visualizer {

class Transform : public Object {
public:
    explicit Transform(std::shared_ptr<Object> object);

    void draw(Batch &batch, const glm::mat4& model) const override;

private:
    std::shared_ptr<Object> object;

    virtual glm::mat4 get_transform(const glm::mat4& model) const = 0;
};

class Scale : public Transform {
public:
    Scale(std::shared_ptr<Object> object, const float &factor);
private:
    const float *factor;

    glm::mat4 get_transform(const glm::mat4& model) const override;
};

class Deform : public Transform {
public:
    Deform(std::shared_ptr<Object> object, const float &width, const float &height);
private:
    const float *width;
    const float *height;

    glm::mat4 get_transform(const glm::mat4& model) const override;
};

class Rotate : public Transform {
public:
    Rotate(std::shared_ptr<Object> object, const float &degree);

private:
    const float *degree;

    glm::mat4 get_transform(const glm::mat4& model) const override;
};

class Translate : public Transform {
public:
    Translate(std::shared_ptr<Object> object, const glm::vec3 &direction);
    Translate(std::shared_ptr<Object> object, float x, float y, const float &z);

private:
    float x;
    float y;
    float z;
    const float *rz;

    glm::mat4 get_transform(const glm::mat4& model) const override;
};

}
