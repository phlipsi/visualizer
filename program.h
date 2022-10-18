#pragma once

#include "shader.h"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <stdexcept>

class Program {
public:
    class Exception : public std::runtime_error {
    public:
        Exception(const char *message);
    };

    Program();
    ~Program();

    void bind(GLuint index, const GLchar *name) const;
    void attach(Shader &&) const;
    void link() const;
    void use() const;

    void set_uniform(const char *name, const glm::mat4x4 &m, bool transpose = false) const;
    void set_uniform(const char *name, const glm::mat2 &m, bool transpose = false) const;
    void set_uniform(const char *name, const glm::vec3 &v) const;
    void set_uniform(const char *name, GLint x) const;
    void set_uniform(const char *name, GLfloat x) const;

    Program(const Program &) = delete;
    Program &operator = (const Program &) = delete;
private:
    GLuint id;
};
