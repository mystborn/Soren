#include "run.h"

char* title = "Camera Playground";

static RectF camera;
static RectF bounds;
static SDL_Texture* inner;
static SDL_Texture* outer;

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    bounds.w = (float)width;
    bounds.h = (float)height;
    camera.w = (float)(width / 2);
    camera.h = (float)(height / 2);

    outer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width / 2, height / 2);
    inner = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width / 2, height / 2);
    SOREN_SDL_ASSERT(outer && inner);

    SDL_SetTextureBlendMode(outer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(inner, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, inner);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &(RectF){ 32, 32, 64, 64 });
    // SDL_SetTextureAlphaMod(inner, 127);

    SDL_SetRenderTarget(renderer, outer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);
    // SDL_RenderRect(renderer, &bounds);

    SDL_SetRenderTarget(renderer, NULL);
}

void game_update(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    SDL_RenderTexture(renderer, outer, NULL, NULL);
    SDL_RenderTexture(renderer, inner, &camera, &camera);
}