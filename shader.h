#pragma once

#include <GL/glew.h>

#include <stdexcept>

class Shader {
public:
    class Exception : public std::runtime_error {
    public:
        Exception(const char *message);
    };

    explicit Shader(GLenum type, const char *source);
    ~Shader();

    Shader(Shader &&shader);
    Shader &operator = (Shader &&shader);

    GLuint get_id() const { return id; }

    Shader(const Shader &) = delete;
    Shader operator = (const Shader &) = delete;

private:
    GLuint id;
};
