#pragma once

#include <buffer.h>
#include <vertexarray.h>

class Cube {
public:
    static const int ATTRIBUTE_POSITION;
    static const int ATTRIBUTE_NORMAL;
    static const int ATTRIBUTE_TEXTURE_COORD;

    Cube();

    void draw() const;
private:
    VertexArray vao;
    Buffer vertices;
};
