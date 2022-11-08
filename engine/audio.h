#pragma once

#include <SDL.h>

#include <functional>

class Audio {
public:
    class Lock {
    public:
        Lock(SDL_AudioDeviceID id);
        ~Lock();

        Lock(const Lock &) = delete;
        Lock(Lock &&) = delete;
        Lock &operator = (const Lock &) = delete;
        Lock &operator = (Lock &&) = delete;
    private:
        SDL_AudioDeviceID id;
    };

    explicit Audio(const SDL_AudioSpec &spec);
    ~Audio();

    void set_callback(std::function<void(Uint8 *, int)> callback);
    const SDL_AudioSpec &get_spec() const;
    void pause(bool wait) const;
    Lock lock() const;
private:
    SDL_AudioDeviceID id;
    SDL_AudioSpec spec;
    std::function<void(Uint8 *, int)> callback;

    static void internal_callback(void *userdata, Uint8 *data, int len);
};