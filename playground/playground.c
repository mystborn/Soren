
#include <stdio.h>
#include <stdlib.h>

#include <ecs.h>
#include <soren_input.h>
#include <timing/soren_stopwatch.h>
#include <graphics/soren_primitives.h>
#include <graphics/text/soren_font.h>

#include <SDL3/SDL.h>

#define GAME_TICKS_PER_FRAME 1000 / 60

static FontInterface font = {0};
static String fps_string = STRING_EMPTY_STATIC;
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

void print_fps(float delta, SDL_Renderer* renderer) {
    static int i = 0;
    static float delta_over_time[120];
    static float calculated_delta = 0;

    if (i == 120) {
        i = 0;
    }

    delta_over_time[i++] = delta;
    if (i % 15 == 0) {
        for (int i = 0; i < 120; i++) {
            calculated_delta += delta_over_time[i];
        }

        calculated_delta /= 120;
    }

    string_clear(&fps_string);
    string_format(&fps_string, "FgPS: %g\n", 1.f / calculated_delta);
    // puts(string_data(&fps_string));

    // font_draw_string(&font, renderer, &fps_string, vector_create(32, 200), (SDL_Color){ 255, 255, 255, 255 });
    font_draw_ext(
        &font,
        renderer,
        string_data(&fps_string),
        string_size(&fps_string),
        vector_create(32, 200), 
        (SDL_Color){ 255, 255, 255, 255 },
        degrees_to_radians(-10),
        vector_create(0, font_line_height(&font) / 2),
        vector_create(2, 2),
        SDL_FLIP_NONE,
        false);

    // font_draw_string_ext(
    //     &font,
    //     renderer,
    //     &fps_string,
    //     vector_create(32, 200), 
    //     (SDL_Color){ 255, 255, 255, 255 },
    //     degrees_to_radians(-10),
    //     vector_create(32, 32),
    //     VECTOR_ONE,
    //     SDL_FLIP_NONE,
    //     false);

    // printf("%g\n", delta);
    // printf("FPS: %g\n", 1.f / calculated_delta);
}

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

void run(SDL_Window* window, SDL_Renderer* renderer) {
    bool quit = false;
    SDL_Event e;

    Stopwatch fps_timer;
    stopwatch_init(&fps_timer);

    Stopwatch cap_timer;
    stopwatch_init(&cap_timer);

    stopwatch_start(&fps_timer);
    int64_t prev_time = 0;
    float delta = 0;

    while(!quit) {
        stopwatch_start(&cap_timer);

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_GAMEPAD_ADDED:
                case SDL_EVENT_GAMEPAD_REMAPPED:
                case SDL_EVENT_MOUSE_WHEEL:
                    input_manager_event(&e);
                    break;
                default:
                    break;
            }
        }

        if (quit) {
            puts("quitting");
            break;
        }

        input_manager_update();

        uint64_t temp = stopwatch_ticks(&fps_timer);
        delta = (temp - prev_time) / 1000.f;
        prev_time = temp;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        print_fps(delta, renderer);
        draw_shapes(renderer);

        SDL_RenderPresent(renderer);

        uint64_t frame_ticks = stopwatch_ticks(&cap_timer);
        if (frame_ticks < GAME_TICKS_PER_FRAME) {
            SDL_Delay((uint32_t)(GAME_TICKS_PER_FRAME - frame_ticks));
        }

        stopwatch_stop(&cap_timer);
    }
}

int main(int argc, char** argv) {
    if (SDL_SetMemoryFunctions(soren_malloc, soren_calloc, soren_realloc, soren_free) < 0) {
        printf("Could not set custom SDL alloc functions! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    ecs_init();
    input_manager_init();

    int width =  800;
    int height = 450;

    SDL_Window* window = SDL_CreateWindow(
        "Playground",
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        printf("Game could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    TTF_Font* ttf = TTF_OpenFont("ATypewriterForMe.ttf", 32);
    if (!ttf) {
        printf("Could not load font! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    font_init_ttf(&font, ttf, false);

    run(window, renderer);

    TTF_CloseFont(ttf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}