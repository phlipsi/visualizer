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

    class Usage {
    public:
        Usage(const Program &program);
        ~Usage();

        void set_uniform(const char *name, const glm::mat4x4 &m, bool transpose = false) const;
        void set_uniform(const char *name, const glm::mat2 &m, bool transpose = false) const;
        void set_uniform(const char *name, const glm::vec3 &v) const;
        void set_uniform(const char *name, GLint x) const;
        void set_uniform(const char *name, GLfloat x) const;

        Usage(Usage &&) = delete;
        Usage(const Usage &) = delete;
        Usage &operator = (Usage &&) = delete;
        Usage &operator = (const Usage &) = delete;
    private:
        const Program *program;
    };

    Program();
    ~Program();

    void bind(GLuint index, const GLchar *name) const;
    void attach(const Shader &) const;
    void link() const;
    Usage use() const;

    Program(const Program &) = delete;
    Program &operator = (const Program &) = delete;
private:
    GLuint id;
};
