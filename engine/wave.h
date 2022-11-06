#pragma once

#include <SDL.h>

#include <string>
#include <vector>

class Wave {
public:
    explicit Wave(const std::string &filename);
    ~Wave();

    Wave(const Wave &) = delete;
    Wave &operator = (const Wave &) = delete;

    const SDL_AudioSpec &get_spec() const;
    const Uint8 *get_buffer() const;
    Uint32 get_length() const;

    std::vector<Uint8> convert_to_spec(const SDL_AudioSpec &destination) const;
private:
    SDL_AudioSpec spec;
    Uint8 *buffer;
    Uint32 length;

    Wave(const SDL_AudioSpec &spec, Uint8 *buffer, Uint32 length);
};
