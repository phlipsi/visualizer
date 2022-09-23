#include <SDL.h>
#include <stdlib.h>
#include <functional>
#include <stdexcept>
#include <vector>
#include <cassert>

double sawtooth(double t) {
    double scale = 2.0 / M_PI;
    double sum = sin(M_PI * t);
    sum -= sin(2 * M_PI * t) / 2.0;
    sum += sin(3 * M_PI * t) / 3.0;
    sum -= sin(4 * M_PI * t) / 4.0;
    return scale * sum;
}

double psi(double t) {
    if (t <= 0) {
        return 0;
    } else {
        return exp(-1.0 / t);
    }
}

double phi(double t) {
    return psi(t) / (psi(t) + psi(1 - t));
}

double hat(double t) {
    double r;
    t = modf(t, &r);
    if (t <= 0.5) {
        return t;
    } else {
        return 1.0 - t;
    }
}

class Rectangle {
public:
    Rectangle(SDL_Renderer *renderer, double period)
      : renderer(renderer),
        texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100)),
        period(period)
    {
        if (texture == nullptr) {
            throw std::runtime_error("Can't create texture for rectangle");
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    ~Rectangle() {
        SDL_DestroyTexture(texture);
    }

    void render(double time) const {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        const double width = 10.0 * sin(2 * M_PI * time / period) + 60.0;
        const double height = -5.0 * sin(2 * M_PI * time / period) + 30.0;
        SDL_Rect r = { (100 - width) / 2, (100 - height) / 2, width, height };
        SDL_RenderFillRect(renderer, &r);
    }

    void blit_to(SDL_Texture *destination, int x, int y, double angle) const {
        SDL_SetRenderTarget(renderer, destination);
        SDL_Rect dest = { x - 50, y - 50, 100, 100 };
        SDL_RenderCopyEx(renderer, texture, nullptr, &dest, angle, nullptr, SDL_FLIP_NONE);
    }

    Rectangle(const Rectangle &) = delete;
    Rectangle &operator = (const Rectangle &) = delete;
private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    double period;
};

class RectangleCircle {
public:
    RectangleCircle(SDL_Renderer *renderer, double period)
      : renderer(renderer),
        rectangle(renderer, period * 2.0 / 3.0),
        texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 500, 500)),
        period(period)
    {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    ~RectangleCircle() {
        SDL_DestroyTexture(texture);
    }

    void render(double time) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);
        rectangle.render(time);
        for (int i = 0; i < 12; ++i) {
            const double x = 250.0 + 150.0 * cos(2 * M_PI * i / 12.0);
            const double y = 250.0 + 150.0 * sin(2 * M_PI * i / 12.0);
            const double alpha = 360.0 * i / 12.0 + 180.0 * sawtooth(time / 4000);
            rectangle.blit_to(texture, x, y, alpha);
        }
    }

    void blit_to(SDL_Texture *destination, int x, int y, double angle) const {
        SDL_SetRenderTarget(renderer, destination);
        SDL_Rect dest = { x - 250, y - 250, 500, 500 };
        SDL_RenderCopyEx(renderer, texture, nullptr, &dest, angle, nullptr, SDL_FLIP_NONE);
    }

private:
    SDL_Renderer *renderer;
    Rectangle rectangle;
    SDL_Texture *texture;
    double period;
};

std::vector<uint32_t> probe_texture(SDL_Renderer *renderer, SDL_Texture *texture, int width, int height) {
    std::vector<uint32_t> buffer(width * height);
    SDL_SetRenderTarget(renderer, texture);
    int rc = SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, buffer.data(), width * 4);
    assert(rc == 0);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return EXIT_FAILURE;
    }
    SDL_Window *const win = SDL_CreateWindow("Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Renderer *const renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    RectangleCircle rectangles(renderer, 2000);

    SDL_Event e;
    bool quit = false;
    const uint32_t initial = SDL_GetTicks();
    while (!quit) {
        const uint32_t now = SDL_GetTicks();
        const uint32_t time = now - initial;
        rectangles.render(time);
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
        SDL_RenderClear(renderer);
        const double angle = 180.0 * sawtooth(time / 5000.0);
        //const double angle = (70 * 2 * M_PI / 10000) * time + 90 * sin(2 * M_PI * time / 10000);
        rectangles.blit_to(nullptr, 320, 240, angle);
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                SDL_KeyboardEvent keyEvent = e.key;
                switch (keyEvent.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    return 0;
}