#pragma once

#include "shader.h"

#include <GL/glew.h>

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

    GLint get_uniform_location(const GLchar *name) const;

    Program(const Program &) = delete;
    Program &operator = (const Program &) = delete;
private:
    GLuint id;
};
