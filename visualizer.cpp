#include <SDL.h>
#include <stdlib.h>

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
    SDL_Renderer *const renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture *const rect = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 100, 100);
    if (rect == nullptr) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(rect, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect r = { 10, 10, 80, 80 };
    SDL_RenderFillRect(renderer, &r);

    SDL_SetRenderTarget(renderer, nullptr);

    SDL_Event e;
    bool quit = false;
    double angle = 0;
    uint32_t last = SDL_GetTicks();
    while (!quit) {
        SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
        SDL_RenderClear(renderer);
        SDL_Rect dest = { 270, 190, 80, 80 };
        SDL_RenderCopyEx(renderer, rect, nullptr, &dest, angle, nullptr, SDL_FLIP_NONE);
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            }
        }
        uint32_t next = SDL_GetTicks();
        angle += 0.1 * (next - last);
        last = next;
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    return 0;
}