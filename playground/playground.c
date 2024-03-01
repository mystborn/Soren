
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

static float angle = 0;

void draw_shapes(SDL_Renderer* renderer) {
    RectF rect = (RectF){ 32, 32, 64, 64 };
    RectF rect2 = (RectF){ 120, 32, 64, 64 };
    draw_rect_rgba(renderer, rect, 255, 0, 0, 255);
    draw_filled_rect_rgba(renderer, rect2, 0, 255, 0, 255);

    angle += 1;

    draw_circle_rgba(renderer, vector_create(64, 128), 16, 1, -1, 0, 0, 255, 255);
    draw_circle_rgba(renderer, vector_create(64, 128), 32, 2, -1, 0, 0, 255, 255);
    draw_circle_rgba(renderer, vector_create(64, 128), 64, 1, -1, 0, 0, 255, 255);
    
    draw_arc_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(angle), -degrees_to_radians(72 + angle), 1, 8, 255, 0, 0, 255);
    draw_arc_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(72 + angle), -degrees_to_radians(144 + angle), 4, 8, 0, 255, 0, 255);

    draw_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(144 + angle), -degrees_to_radians(216 + angle), 1, 8, 0, 0, 255, 255);
    draw_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(216 + angle), -degrees_to_radians(288 + angle), 4, 8, 255, 255, 0, 255);
    draw_filled_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(288 + angle), -degrees_to_radians(360 + angle), 8, 255, 0, 255, 255);
}

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    return;
}

void game_update(SDL_Window* window, SDL_Renderer* renderer) {
    draw_shapes(renderer);
}