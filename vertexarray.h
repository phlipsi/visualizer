#pragma once

#include <GL/glew.h>

class VertexArray {
public:
    class Binding {
    public:
        Binding(const VertexArray &vao);
        ~Binding();

        void enable_attribute(GLuint index);

        Binding(Binding &&) = delete;
        Binding(const Binding &) = delete;
        Binding &operator = (const Binding &) = delete;
        Binding &operator = (Binding &&) = delete;
    private:
        bool bound;
    };

    VertexArray();
    ~VertexArray();

    Binding bind();

    VertexArray(const VertexArray &) = delete;
    VertexArray &operator = (const VertexArray &) = delete;
private:
    GLuint id;
};
