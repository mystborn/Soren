#include "run.h"

#include <stdio.h>
#include <stdlib.h>

#include <ecs.h>
#include <soren_input.h>
#include <timing/soren_stopwatch.h>
#include <graphics/soren_primitives.h>
#include <graphics/text/soren_font.h>

#define GAME_TICKS_PER_FRAME 1000 / 60

FontInterface* font = NULL;

static String fps_string = STRING_EMPTY_STATIC;

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

    if (calculated_delta == 0) {
        return;
    }

    string_clear(&fps_string);
    string_format(&fps_string, "fps: %g\n", 1.f / calculated_delta);

    font_draw(
        font, 
        renderer, 
        string_data(&fps_string), 
        string_size(&fps_string), 
        vector_create(700, 5),
        (SDL_FColor){ 1, 1, 1, 1 });
}

static void run(SDL_Window* window, SDL_Renderer* renderer) {
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

        game_update(window, renderer);

        print_fps(delta, renderer);

        SDL_RenderPresent(renderer);

        uint64_t frame_ticks = stopwatch_ticks(&cap_timer);
        if (frame_ticks < GAME_TICKS_PER_FRAME) {
            SDL_Delay((uint32_t)(GAME_TICKS_PER_FRAME - frame_ticks));
        }

        stopwatch_stop(&cap_timer);
    }
}

int main(int argc, char** argv) {
    e4c_context_begin(false);

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
        title,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        printf("Game could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

    printf("Renderer: %s\n", info.name);

    TTF_Font* ttf = TTF_OpenFont("ATypewriterForMe.ttf", 16);
    if (!ttf) {
        printf("Could not load font! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    font = font_create_ttf(ttf, true);

    try {
        game_init(window, renderer);
        run(window, renderer);
    } catch(RuntimeException) {
        const e4c_exception* exception = e4c_get_exception();
        printf("Encountered a runtime exception :(\n%s:\n%s", exception->name, exception->message);
    }

    font_free(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    e4c_context_end();

    return EXIT_SUCCESS;
}