#include "program.h"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

Program::Exception::Exception(const char *message)
    : std::runtime_error(message) { }

Program::Program()
    : id(glCreateProgram())
{
    if (id == 0) {
        throw Exception("Can't create program object");
    }
}

Program::~Program() {
    glDeleteProgram(id);
}

void Program::bind(GLuint index, const GLchar *name) const {
    glBindAttribLocation(id, index, name);
}

void Program::attach(Shader &&shader) const {
    glAttachShader(id, shader.get_id());

    //GLint status;
    //glGetProgramiv(id, GL_LINK_STATUS, &status);
    //if (status == GL_FALSE) {
    //    std::vector<char> buffer(512);
    //    glGetProgramInfoLog(id, 512, NULL, buffer.data());
    //    throw Exception(buffer.data());
    //}
}

void Program::link() const {
    glLinkProgram(id);
}

void Program::use() const {
    glUseProgram(id);
}

void Program::set_uniform(const char *name, const glm::mat4x4 &m, bool transpose) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(m));
}

void Program::set_uniform(const char *name, const glm::mat2x4 &m, bool transpose) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(m));
}
