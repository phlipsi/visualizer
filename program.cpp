#include "program.h"

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

GLint Program::get_uniform_location(const GLchar *name) const {
    return glGetUniformLocation(id, name);
}
