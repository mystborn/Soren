#include "run.h"
#include <graphics/soren_camera.h>
#include <graphics/soren_primitives.h>
#include <graphics/soren_graphics.h>

char* title = "Camera Playground";

static RectF viewport;
static RectF player;
static Camera* camera;

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    viewport.x = 0;
    viewport.y = 0;
    viewport.w = (float)width;
    viewport.h = (float)height;

    camera = camera_create(renderer, width / 2, height / 2);
    camera_set_x(camera, 64);
    camera_set_rotation(camera, degrees_to_radians(10));

    player.y = camera->bounds.h / 4;
    player.h = camera->bounds.h / 2;
    player.w = camera->bounds.h / 2;
    
    graphics_set_camera(camera);
}

void game_update(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, camera->render_target);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
    SDL_RenderClear(renderer);

    player.x += 3;
    if (player.x + player.w > camera->bounds.w) {
        player.x = 0;
    }

    draw_filled_rect_color(renderer, player, (SDL_FColor){ 1, 0, 0, 1 });

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderTexture(renderer, camera->render_target, &(RectF){ 0, 0, camera->bounds.w, camera->bounds.h }, &viewport);
}