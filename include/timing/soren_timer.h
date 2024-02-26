#ifndef SOREN_TIMING_SOREN_TIMER_H
#define SOREN_TIMING_SOREN_TIMER_H

typedef enum TimerState {
    SOREN_TIMER_STOPPED,
    SOREN_TIMER_STARTED,
    SOREN_TIMER_PAUSED,
    SOREN_TIMER_COMPLETED
} TimerState;

typedef struct Timer {
    int timer_type;
    float current_time;
    float interval;
} Timer;

#endif