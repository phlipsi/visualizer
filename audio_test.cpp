#include <SDL.h>

#include <algorithm>
#include <iostream>
#include <cstddef>

struct Buffer {
    Uint8 *start;
    Uint32 length;
    Uint32 offset;
    SDL_AudioFormat format;
};

void query_audio(void *userdata, Uint8 *data, int len) {
    Buffer *const buffer = static_cast<Buffer *>(userdata);

    if (buffer->offset >= buffer->length) {
        return;
    }
    len = std::min<unsigned long long>(len, buffer->length - buffer->offset);
    memset(data, 0, len);
    SDL_MixAudioFormat(data, buffer->start + buffer->offset, buffer->format, len, SDL_MIX_MAXVOLUME);
    buffer->offset += len;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_AudioSpec spec;
    Uint8 *buffer;
    Uint32 length;
    if (SDL_LoadWAV("C:\\Users\\pwaehnert\\Downloads\\dream.wav", &spec, &buffer, &length) == nullptr) {
        return EXIT_FAILURE;
    }

    const int num_audio_dev = SDL_GetNumAudioDevices(false);
    for (int i = 0; i < num_audio_dev; ++i) {
        std::cout << "Audio device " << i << ": " << SDL_GetAudioDeviceName(i, 0) << '\n';
    }

    Buffer b = { buffer, length, 0, spec.format };
    SDL_AudioSpec internal_spec = spec;
    internal_spec.callback = query_audio;
    internal_spec.userdata = &b;
    const int id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(2, 0), 0, &internal_spec, &spec, 0);
    if (id < 0) {
        return EXIT_FAILURE;
    }

    SDL_PauseAudioDevice(id, 0);
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                break;
            }
        }
    }

    SDL_CloseAudioDevice(id);
    SDL_FreeWAV(buffer);
    SDL_Quit();
    return EXIT_SUCCESS;
}
