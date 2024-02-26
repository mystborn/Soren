
#include <stdio.h>
#include <stdlib.h>

#include <ecs.h>
#include <soren_input.h>
#include <timing/soren_stopwatch.h>

#include <SDL3/SDL.h>

#define GAME_TICKS_PER_FRAME 1000 / 60

void run(SDL_Window* window) {
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

        uint64_t frame_ticks = stopwatch_ticks(&cap_timer);
        if (frame_ticks / GAME_TICKS_PER_FRAME) {
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

    ecs_init();
    input_manager_init();

    int width =  800;
    int height = 450;

    SDL_Window* window = SDL_CreateWindow(
        "Playground",
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        printf("Game could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    run(window);

    SDL_DestroyWindow(window);

    return EXIT_SUCCESS;
}