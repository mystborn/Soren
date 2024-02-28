#include "run.h"
#include <graphics/soren_primitives.h>

char* title = "Camera Playground";

static RectF camera;
static RectF bounds;
static RectF middle;
static RectF player;
static SDL_Texture* inner;
static SDL_Texture* outer;
static float angle = 0;
static String logger = STRING_EMPTY_STATIC;

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    bounds.w = (float)width;
    bounds.h = (float)height;
    camera.w = (float)(width / 2);
    camera.h = (float)(height / 2);
    middle.x = (float)width / 4;
    middle.y = (float)height / 4;
    middle.w = camera.w;
    middle.h = camera.h;
    player.x = 0;
    player.y = camera.h / 4;
    player.w = camera.h / 2;
    player.h = camera.h / 2;

    outer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width / 2, height / 2);
    inner = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width / 2, height / 2);
    SOREN_SDL_ASSERT(outer && inner);

    SDL_SetTextureBlendMode(outer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(inner, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, inner);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, outer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 127);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, NULL);
}

void game_update(SDL_Window* window, SDL_Renderer* renderer) {
    string_clear(&logger);
    player.x += 3;
    if (player.x + player.w > camera.w) {
        player.x = 0;
    }

    angle += 1;
    if (angle >= 360) {
        angle = 0;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

    SDL_SetRenderTarget(renderer, inner);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    Vector origin = rectf_center(camera);
    Matrix transform = matrix_create_tro(VECTOR_ZERO, degrees_to_radians(angle), origin);

    Vector points[4] = {
        { player.x, player.y },
        { player.x + player.w, player.y },
        { player.x + player.w, player.y + player.h },
        { player.x, player.y + player.h }
    };

    vector_transform_batch(points, 4, points, &transform);

    draw_filled_rect_rgba(renderer, player, 0, 255, 0, 127);
    draw_polygon_rgba(renderer, points, 4, 255, 255, 0, 127);

    SDL_SetRenderTarget(renderer, NULL);
    
    SDL_RenderTexture(renderer, inner, &camera, &middle);
    SDL_RenderTextureRotated(
        renderer,
        outer,
        &camera,
        &middle,
        angle,
        NULL,
        SDL_FLIP_NONE
    );
}