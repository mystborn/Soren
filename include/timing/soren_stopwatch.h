#ifndef SOREN_TIMING_SOREN_STOPWATCH_H
#define SOREN_TIMING_SOREN_STOPWATCH_H

#include <stdint.h>

#include "soren_std.h"
#include "soren_timer.h"

#include <SDL3/SDL.h>

typedef struct Stopwatch {
    uint64_t start_ticks;
    uint64_t paused_ticks;
    TimerState state;
} Stopwatch;

static inline void stopwatch_init(Stopwatch* stopwatch) {
    soren_assert(stopwatch);

    *stopwatch = (Stopwatch){0};
}

static inline Stopwatch* stopwatch_create(void) {
    Stopwatch* watch = soren_malloc(sizeof(*watch));
    if (!watch)
        return NULL;

    stopwatch_init(watch);
    return watch;
}

static inline void stopwatch_free(Stopwatch* stopwatch) {
    soren_free(stopwatch);
}

static inline void stopwatch_start(Stopwatch* stopwatch) {
    stopwatch->state = SOREN_TIMER_STARTED;
    stopwatch->paused_ticks = 0;
    stopwatch->start_ticks = SDL_GetTicks();
}

static inline void stopwatch_stop(Stopwatch* stopwatch) {
    *stopwatch = (Stopwatch){0};
}

static inline void stopwatch_pause(Stopwatch* stopwatch) {
    if (stopwatch->state != SOREN_TIMER_STARTED) {
        return;
    }

    stopwatch->paused_ticks = SDL_GetTicks() - stopwatch->start_ticks;
    stopwatch->state = SOREN_TIMER_PAUSED;
    stopwatch->start_ticks = 0;
}

static inline void stopwatch_resume(Stopwatch* stopwatch) {
    if (stopwatch->state != SOREN_TIMER_PAUSED) {
        return;
    }

    stopwatch->state = SOREN_TIMER_STARTED;
    stopwatch->start_ticks = SDL_GetTicks() - stopwatch->paused_ticks;
    stopwatch->paused_ticks = 0;
}

static inline uint64_t stopwatch_ticks(Stopwatch* stopwatch) {
    switch (stopwatch->state) {
        case SOREN_TIMER_STARTED:
            return SDL_GetTicks() - stopwatch->start_ticks;
        case SOREN_TIMER_PAUSED:
            return stopwatch->paused_ticks;
        default:
            return 0;
    }
}

static inline TimerState stopwatch_state(Stopwatch* stopwatch) {
    return stopwatch->state;
}

#endif