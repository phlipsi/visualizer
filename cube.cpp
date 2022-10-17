#include "cube.h"

namespace {

//       (++-)    (+++)
//           *--------*
//          /|       /|
//    (+--)/ | (+-+)/ |
//        *--------*  |
//        |  *-----|--*
//        | /(-+-) | /(-++)
//        |/       |/
//        *--------*
//        (---)    (--+)
//    x
//    |  y
//    | /
//    |/
//    +-----z
GLfloat cube_vertices[] = {
    /*  X      Y      Z      NX     NY     NZ    S     T  */
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // front
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // back
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,

       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // left
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,

       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // right
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,

        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
};

}

const int Cube::ATTRIBUTE_POSITION = 0;
const int Cube::ATTRIBUTE_NORMAL = 1;
const int Cube::ATTRIBUTE_TEXTURE_COORD = 2;

Cube::Cube() {
    auto binding = vao.bind();
    binding.enable_attribute(ATTRIBUTE_POSITION);
    binding.enable_attribute(ATTRIBUTE_NORMAL);
    binding.enable_attribute(ATTRIBUTE_TEXTURE_COORD);

    auto buffer_binding = binding.bind(GL_ARRAY_BUFFER, vertices);
    buffer_binding.buffer_data(sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    buffer_binding.vertex_attrib_pointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(0 * sizeof(float)));
    buffer_binding.vertex_attrib_pointer(ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(3 * sizeof(float)));
    buffer_binding.vertex_attrib_pointer(ATTRIBUTE_TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(6 * sizeof(float)));
}

void Cube::draw() const {
    auto binding = vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));
}
