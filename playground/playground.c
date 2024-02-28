
#include "run.h"
#include "soren_math.h"
#include "graphics/soren_primitives.h"

char* title = "Playground";

static Vector hexagon[6] = {
    { 700, 32 },
    { 732, 64 },
    { 732, 96 },
    { 700, 128 },
    { 668, 96 },
    { 668, 64 }
};

static Vector octagon[8] = {
    { 600, 32 },
    { 632, 32 },
    { 664, 64 },
    { 664, 96 },
    { 632, 128 },
    { 600, 128 },
    { 568, 96 },
    { 568, 64 }
};

static Vector bowtie[6] = {
    { 600, 200 },
    { 650, 225 },
    { 700, 200 },
    { 700, 300 },
    { 650, 275 },
    { 600, 300 }
};

void draw_shapes(SDL_Renderer* renderer) {
    RectF rect = (RectF){ 32, 32, 64, 64 };
    RectF rect2 = (RectF){ 120, 32, 64, 64 };
    draw_rect_rgba(renderer, rect, 255, 0, 0, 255);
    draw_filled_rect_rgba(renderer, rect2, 0, 255, 0, 255);

    draw_circle_rgba(renderer, vector_create(64, 128), 32, 0, 0, 255, 255);
    draw_polygon_rgba(renderer, hexagon, 6, 0, 255, 255, 255);
    draw_filled_convex_polygon_rgba(renderer, octagon, 8, 255, 0, 255, 255);
    draw_filled_concave_polygon_rgba(renderer, bowtie, 6, 0, 255, 255, 255);
}

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    return;
}

void game_update(SDL_Window* window, SDL_Renderer* renderer) {
    draw_shapes(renderer);
}