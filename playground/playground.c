
#include "run.h"
#include <soren_math.h>
#include <graphics/soren_graphics.h>
#include <graphics/soren_primitives.h>
#include <graphics/soren_sprite.h>
#include <SDL3_image/SDL_image.h>

char* title = "Playground";

SDL_Texture* image;
SpriteAnimator animator;

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

void draw_shapes(SDL_Renderer* renderer, float delta) {
    RectF rect = (RectF){ 32, 32, 64, 64 };
    RectF rect2 = (RectF){ 120, 32, 64, 64 };
    draw_rect_color(renderer, rect, soren_colors.red);
    draw_filled_rect_color(renderer, rect2, soren_colors.green);

    draw_polygon_color(renderer, hexagon, 6, soren_colors.purple);
    draw_filled_convex_polygon_color(renderer, octagon, 8, soren_colors.cyan);
    draw_filled_concave_polygon_color(renderer, bowtie, 6, soren_colors.saddle_brown);

    angle += 1;

    draw_circle_color(renderer, vector_create(64, 128), 16, 1, -1, soren_colors.blue);
    draw_circle_color(renderer, vector_create(64, 128), 32, 2, -1, soren_colors.steel_blue);
    draw_circle_color(renderer, vector_create(64, 128), 64, 1, -1, soren_colors.light_blue);
    
    draw_arc_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(angle), -degrees_to_radians(72 + angle), 1, 8, 255, 0, 0, 255);
    draw_arc_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(72 + angle), -degrees_to_radians(144 + angle), 4, 8, 0, 255, 0, 255);

    draw_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(144 + angle), -degrees_to_radians(216 + angle), 1, 8, 0, 0, 255, 255);
    draw_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(216 + angle), -degrees_to_radians(288 + angle), 4, 8, 255, 255, 0, 255);
    draw_filled_pie_rgba(renderer, vector_create(400, 225), 64, -degrees_to_radians(288 + angle), -degrees_to_radians(360 + angle), 8, 255, 0, 255, 255);

    // SDL_RenderTexture(renderer, image, NULL, &(RectF){ 600, 300, 64, 64 });

    // sprite_draw_ext(sprite, renderer, 0, vector_create(600, 300), soren_colors.white, degrees_to_radians(-10), VECTOR_ONE, SDL_FLIP_NONE);
    // sprite_draw_ext(sprite, renderer, 0, vector_create(600, 300), soren_colors.white, degrees_to_radians(-10), vector_create(2, 2), SDL_FLIP_VERTICAL);
    // sprite_draw_ext(sprite, renderer, 0, vector_create(600, 300), soren_colors.white, 0, VECTOR_ONE, SDL_FLIP_NONE);

    sprite_animator_update(&animator, delta);
    sprite_animator_draw_pos_ext(&animator, renderer, vector_create(600, 300), soren_colors.white, degrees_to_radians(-angle), vector_create(2, 2), SDL_FLIP_VERTICAL);
}

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    image = IMG_LoadTexture(renderer, "hills.png");
    SpriteAtlas* atlas = sprite_atlas_create_from_json_file(renderer, "sprite.json");
    
    Sprite* sprite = sprite_list_get(atlas->animation_list, 0);
    sprite_animator_set_sprite(&animator, sprite);
}

void game_update(SDL_Window* window, SDL_Renderer* renderer, float delta) {
    draw_shapes(renderer, delta);
}