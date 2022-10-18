#include "shader.h"

#include <cstring>
#include <vector>

Shader::Exception::Exception(const char *message)
    : std::runtime_error(message) { }

Shader::Shader(GLenum type, const char *source)
    : id(glCreateShader(type)) {
    if (id == 0) {
        throw Exception("Can't create shader object");
    }
    const GLchar *const sources[] = { source };
    const GLint source_lengths[] = { static_cast<GLint>(strlen(source)) };
    glShaderSource(id, 1, sources, source_lengths);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> buffer(length);
        glGetShaderInfoLog(id, length, NULL, buffer.data());
        throw Exception(buffer.data());
    }
}

Shader::~Shader() {
    if (id != 0) {
        glDeleteShader(id);
    }
}

Shader::Shader(Shader &&shader)
    : id(shader.id)
{
    shader.id = 0;
}

Shader &Shader::operator = (Shader &&shader) {
    id = shader.id;
    shader.id = 0;
    return *this;
}
