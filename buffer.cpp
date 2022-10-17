#include "buffer.h"

Buffer::Buffer() {
    glGenBuffers(1, &id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}
