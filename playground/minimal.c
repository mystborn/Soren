#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 450

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Minimal example",
        WIDTH,
        HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        printf("Game could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

    printf("Renderer: %s\n", info.name);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 225);
    SDL_SetRenderTarget(renderer, texture);

    printf("%p == %p\n", texture, SDL_GetRenderTarget(renderer));

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}