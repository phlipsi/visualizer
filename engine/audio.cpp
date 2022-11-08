#include "audio.h"

#include <iostream>
#include <stdexcept>

Audio::Lock::Lock(SDL_AudioDeviceID id)
  : id(id)
{
    SDL_LockAudioDevice(id);
}

Audio::Lock::~Lock() {
    SDL_UnlockAudioDevice(id);
}

Audio::Audio(const SDL_AudioSpec &spec)
{
    int num_audio_dev = SDL_GetNumAudioDevices(false);
    for (int i = 0; i < num_audio_dev; ++i) {
        std::cout << "Audio device " << i << ": " << SDL_GetAudioDeviceName(i, 0) << '\n';
    }

    SDL_AudioSpec internal_spec = spec;
    internal_spec.callback = internal_callback;
    internal_spec.userdata = this;
    id = SDL_OpenAudioDevice(nullptr, 0, &internal_spec, &this->spec, 0);
    if (id < 0) {
        throw std::runtime_error("Can't open audio device");
    }
}

Audio::~Audio() {
    SDL_CloseAudioDevice(id);
}

void Audio::set_callback(std::function<void(Uint8 *, int)> callback) {
    this->callback = callback;
}

const SDL_AudioSpec &Audio::get_spec() const {
    return spec;
}

void Audio::pause(bool wait) const {
    SDL_PauseAudioDevice(id, wait);
}

Audio::Lock Audio::lock() const {
    return Lock(id);
}

void Audio::internal_callback(void *userdata, Uint8 *data, int len) {
    Audio *const self = static_cast<Audio *>(userdata);
    if (self->callback) {
        self->callback(data, len);
    }
}
