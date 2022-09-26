#include <SDL.h>
#include <stdlib.h>
#include <functional>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <SDL2_gfxPrimitives.h>

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

class Parameters {
public:
    virtual ~Parameters() = default;

    virtual uint32_t get_rectangle_color(double time) const = 0;
    virtual double get_rectangle_deformation(double time) const = 0;
    virtual double get_rectangle_rotation(double time) const = 0;

    virtual double get_triangle_deformation(double time) const = 0;
    virtual double get_triangle_rotation(double time) const = 0;
};

inline uint8_t get_red(uint32_t rgba) {
    return (rgba & 0xff000000u) >> 24;
}

inline uint8_t get_green(uint32_t rgba) {
    return (rgba & 0x00ff0000u) >> 16;
}

inline uint8_t get_blue(uint32_t rgba) {
    return (rgba & 0x0000ff00u) >> 8;
}

inline uint8_t get_alpha(uint32_t rgba) {
    return rgba & 0x000000ffu;
}

inline uint32_t get_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (r << 24) | (g << 16) | (b << 8) | a;
}

class Item {
public:
    explicit Item(SDL_Renderer *renderer, int width, int height)
      : renderer(renderer),
        texture(SDL_CreateTexture(renderer,
                                  SDL_PIXELFORMAT_RGBA8888,
                                  SDL_TEXTUREACCESS_TARGET,
                                  width, height)),
        width(width),
        height(height)
    {
        if (texture == nullptr) {
            throw std::runtime_error("Can't create texture for rectangle");
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    ~Item() {
        SDL_DestroyTexture(texture);
    }

    Item(const Item &) = delete;
    Item &operator = (const Item &) = delete;

    void render() const {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);
    }

    void blit_to(SDL_Texture *destination, int x, int y, double scale, double angle) const {
        SDL_SetRenderTarget(renderer, destination);
        SDL_Rect dest = { static_cast<int>(x - scale * width / 2),
                          static_cast<int>(y - scale * height / 2),
                          static_cast<int>(scale * width),
                          static_cast<int>(scale * height) };
        SDL_RenderCopyEx(renderer,
                         texture,
                         nullptr,
                         &dest,
                         angle,
                         nullptr,
                         SDL_FLIP_NONE);
    }

protected:
    SDL_Renderer *get_renderer() const {
        return renderer;
    }

    SDL_Texture *get_texture() const {
        return texture;
    }

private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width;
    int height;
};

class Circle : public Item {
public:
    Circle(SDL_Renderer *renderer, double period)
      : Item(renderer, 100, 100),
        period(period)
    { }

    void render(double time) const {
        Item::render();
        SDL_SetRenderDrawColor(get_renderer(), 255, 0, 0, 255);
        const double radius = 10.0 * sin(2 * M_PI * time / period) + 40.0;
        filledCircleRGBA(get_renderer(), 50, 50, static_cast<Sint16>(radius), 100, 100, 100, SDL_ALPHA_OPAQUE);
    }

private:
    double period;
};

class Rectangle : public Item {
public:
    Rectangle(SDL_Renderer *renderer, const Parameters &params)
      : Item(renderer, 100, 100),
        params(&params)
    { }

    void render(double time) const {
        Item::render();
        const uint32_t color = params->get_rectangle_color(time);
        SDL_SetRenderDrawColor(get_renderer(), get_red(color), get_green(color), get_blue(color), get_alpha(color));
        const double deformation = params->get_rectangle_deformation(time);
        const double current_width = 60.0 * (1.0 + deformation);
        const double current_height = 30.0 * (1.0 - deformation);
        SDL_Rect r = { static_cast<int>((100 - current_width) / 2),
                       static_cast<int>((100 - current_height) / 2),
                       static_cast<int>(current_width),
                       static_cast<int>(current_height) };
        SDL_RenderFillRect(get_renderer(), &r);
    }

private:
    const Parameters *params;
};

class Triangle : public Item {
public:
    Triangle(SDL_Renderer *renderer, const Parameters &params)
      : Item(renderer, 100, 100),
        params(&params)
    { }

    void render(double time) const {
        Item::render();
        SDL_SetRenderDrawColor(get_renderer(), 255, 0, 0, 255);
        const double deformation = params->get_triangle_deformation(time);
        const double width = 60.0 * (1.0 + deformation);
        const double height = 60.0 * (1.0 - deformation);
        filledTrigonRGBA(get_renderer(),
                         static_cast<Sint16>(50 - width / 2), static_cast<Sint16>(50 + height / 2),
                         static_cast<Sint16>(50 + width / 2), static_cast<Sint16>(50 + height / 2),
                         50, static_cast<Sint16>(50 - height / 2),
                         255, 0, 0, 255);
    }

private:
    const Parameters *params;
    double period;
};

class RectangleCircle : public Item {
public:
    RectangleCircle(SDL_Renderer *renderer, const Parameters &params, double period)
      : Item(renderer, 500, 500),
        params(&params),
        rectangle(renderer, params),
        triangle(renderer, params)
    { }

    void render(double time) {
        Item::render();
        rectangle.render(time);
        triangle.render(time);
        for (int i = 0; i < 6; ++i) {
            const double x = 250.0 + 150.0 * cos(2 * M_PI * i / 6.0);
            const double y = 250.0 + 150.0 * sin(2 * M_PI * i / 6.0);
            const double alpha = 360.0 * i / 6.0 + params->get_rectangle_rotation(time);
            rectangle.blit_to(get_texture(), static_cast<int>(x), static_cast<int>(y), 1.0, alpha);

            const double x1 = 250.0 + 150.0 * cos(2 * M_PI * (i + 0.5) / 6.0);
            const double y1 = 250.0 + 150.0 * sin(2 * M_PI * (i + 0.5) / 6.0);
            const double alpha1 = 360.0 * (i + 0.5) / 6.0 + params->get_triangle_rotation(time);
            triangle.blit_to(get_texture(), static_cast<int>(x1), static_cast<int>(y1), 1.0, alpha1);
        }
    }

private:
    const Parameters *params;
    Rectangle rectangle;
    Triangle triangle;
    double period;
};

class Circles : public Item {
public:
    Circles(SDL_Renderer *renderer, double period)
      : Item(renderer, 500, 500),
        circle(renderer, period * 2.0 / 3.0),
        period(period)
    { }

    void render(double time) {
        Item::render();
        circle.render(time);
        for (int i = 0; i < 12; ++i) {
            const double x = 250.0 + 150.0 * cos(2 * M_PI * i / 12.0);
            const double y = 250.0 + 150.0 * sin(2 * M_PI * i / 12.0);
            circle.blit_to(get_texture(), static_cast<int>(x), static_cast<int>(y), 0.3, 0.0);
        }
    }

private:
    Circle circle;
    double period;
};

class ParametersImpl : public Parameters {
public:
    uint32_t get_rectangle_color(double time) const override {
        uint8_t component = static_cast<uint8_t>(127.0 * sawtooth(time / 5000) + 128.0);
        return get_rgba(component, component, 0, SDL_ALPHA_OPAQUE);
    }

    double get_rectangle_deformation(double time) const override {
        return sin(2 * M_PI * time / (2 * 2000.0 / 3)) / 6.0;
    }

    double get_rectangle_rotation(double time) const override {
        return 180.0 * sawtooth(time / 4000);
    }

    double get_triangle_deformation(double time) const override {
        return sin(2 * M_PI * time / (3 * 2000.0 / 4)) / 6.0;
    }

    double get_triangle_rotation(double time) const override {
        return 180.0 * sawtooth(time / 3000);
    }
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    SDL_Renderer *const renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    ParametersImpl params;
    RectangleCircle rectangles(renderer, params, 2000);
    Circles circles(renderer, 2500);

    SDL_Event e;
    bool quit = false;
    const uint32_t initial = SDL_GetTicks();
    while (!quit) {
        const uint32_t now = SDL_GetTicks();
        const uint32_t time = now - initial;
        rectangles.render(time);
        circles.render(time);
        SDL_SetRenderTarget(renderer, nullptr);
        //SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        const double f = 0.06 * sin(2 * M_PI * time / 5000.0) + 1.0;
        const double angle = 180.0 * sawtooth(time / 5000.0);
        //const double angle = (70 * 2 * M_PI / 10000) * time + 90 * sin(2 * M_PI * time / 10000);
        rectangles.blit_to(nullptr, 320, 240, f * 1.0, angle);

        const double f2 = 0.06 * sin(2 * M_PI * (time - 1000.0) / 5000.0) + 1.0;
        const double angle2 = 180.0 * sawtooth(time / 4000.0);
        rectangles.blit_to(nullptr, 320, 240, f2 * 0.35, angle2);

        const double f3 = 0.06 * sin(2 * M_PI * (time - 2000.0) / 5000.0) + 1.0;
        const double angle3 = 180.0 * sawtooth(time / 6000.0);
        rectangles.blit_to(nullptr, 320, 240, f3 * 2.4, angle3);

        const double f4 = 0.06 * sin(2 * M_PI * (time - 3000.0) / 5000.0) + 1.0;
        const double angle4 = 360.0 * time / 5000.0;
        circles.blit_to(nullptr, 320, 240, f4 * 1.55, -angle4);

        const double f5 = 0.06 * sin(2 * M_PI * (time - 4000.0) / 5000.0) + 1.0;
        const double angle5 = 360.0 * time / 7000.0;
        circles.blit_to(nullptr, 320, 240, f5 * 0.6, -angle5);

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
