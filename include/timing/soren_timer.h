#ifndef SOREN_TIMING_SOREN_TIMER_H
#define SOREN_TIMING_SOREN_TIMER_H

#include "soren_std.h"
#include <generic_event.h>

EVENT_DEFINE_0_H(TimerEvent, timer_event)

typedef enum TimerState {
    SOREN_TIMER_STOPPED,
    SOREN_TIMER_STARTED,
    SOREN_TIMER_PAUSED,
    SOREN_TIMER_COMPLETED
} TimerState;

typedef struct Timer {
    TimerEvent tick;
    TimerState state;
    float current_time;
    float interval;
    bool ticked;
    bool repeat;
} Timer;

static inline void timer_init(Timer* timer, float interval, bool repeat) {
    if (interval < 0) {
        throw(IllegalArgumentException, "Interval cannot be < 0");
    }

    timer_event_init(&timer->tick);
    timer->state = SOREN_TIMER_STOPPED;
    timer->current_time = 0;
    timer->interval = interval;
    timer->ticked = false;
    timer->repeat = false;
}

static inline Timer* timer_create(float interval, bool repeat) {
    Timer* timer = soren_malloc(sizeof(*timer));
    if (!timer) {
        return NULL;
    }

    timer_init(timer, interval, repeat);
    return timer;
}

static inline void timer_free_resources(Timer* timer) {
    timer_event_free_resources(&timer->tick);
}

static inline void timer_free(Timer* timer) {
    timer_free_resources(timer);
    soren_free(timer);
}

static inline float timer_current_time(Timer* timer) {
    return timer->current_time;
}

static inline float timer_interval(Timer* timer) {
    return timer->interval;
}

static inline void timer_set_interval(Timer* timer, float interval) {
    timer->interval = interval;
}

static inline TimerState timer_state(Timer* timer) {
    return timer->state;
}

static inline TimerEvent* timer_tick_event(Timer* timer) {
    return &timer->tick;
}

static inline bool timer_ticked(Timer* timer) {
    return timer->ticked;
}

static inline void timer_start(Timer* timer) {
    switch (timer->state) {
        case SOREN_TIMER_STOPPED:
        case SOREN_TIMER_COMPLETED:
            timer->current_time = 0;
            break;
        case SOREN_TIMER_PAUSED:
            break;
        case SOREN_TIMER_STARTED:
            return;
    }

    timer->state = SOREN_TIMER_STARTED;
}

static inline void timer_stop(Timer* timer) {
    timer->state = SOREN_TIMER_STOPPED;
    timer->current_time = 0;
    timer->ticked = false;
}

static inline void timer_restart(Timer* timer) {
    timer_stop(timer);
    timer_start(timer);
}

static inline void timer_pause(Timer* timer) {
    if (timer->state != SOREN_TIMER_STARTED) {
        return;
    }

    timer->ticked = false;
    timer->state = SOREN_TIMER_PAUSED;
}

static inline void timer_update(Timer* timer, float delta_time) {
    timer->ticked = false;

    if (timer->state != SOREN_TIMER_STARTED || timer->interval == 0) {
        return;
    }

    timer->current_time += delta_time;

    while (timer->current_time >= timer->interval) {
        timer->current_time -= timer->interval;
        timer_event_trigger(&timer->tick);
        timer->ticked = true;

        if (!timer->repeat) {
            timer->state = SOREN_TIMER_COMPLETED;
            return;
        }
    }
}

#endif