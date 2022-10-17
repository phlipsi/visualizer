#pragma once

#include <GL/glew.h>

#include <optional>

class Buffer {
public:
    Buffer();
    ~Buffer();

    GLuint get_id() const { return id; }

    Buffer(const Buffer &) = delete;
    Buffer &operator = (const Buffer &) = delete;
private:
    GLuint id;
};
