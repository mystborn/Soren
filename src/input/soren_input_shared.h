#ifndef SOREN_INPUT_SOREN_INPUT_SHARED_H
#define SOREN_INPUT_SOREN_INPUT_SHARED_H

#include <soren_input.h>
#include <soren_generics.h>
#include <ecs_int_dispenser.h>

#define GAMEPAD_BUTTON(x) (1 << (x))

typedef struct ActionMap {
    bool current_pressed;
    bool previous_pressed;
    ScancodeList keys;
    GamepadActionList buttons;
    MouseButtonList mouse_buttons;
} ActionMap;

typedef struct Gamepad {
    SDL_Gamepad* gamepad;
    SDL_JoystickID id;
    uint32_t button_current;
    uint32_t button_previous;
    int16_t left_deadzone;
    int16_t right_deadzone;
    bool active;
} Gamepad;

typedef struct InputManager {
    int key_count;
    const uint8_t* keyboard_current;
    uint8_t* keyboard_previous;
    MouseButtons mouse_current;
    MouseButtons mouse_previous;
    Vector mouse_position_current;
    Vector mouse_position_previous;
    Gamepad gamepads[MAX_GAMEPADS];
    EcsIntDispenser gamepad_index_dispenser;
    IntList* connected_gamepads;
    GamepadConnectionEvent gamepad_connected;
    GamepadConnectionEvent gamepad_disconnected;
    Vector wheel_total;
    Vector wheel_delta;
    Vector wheel_accumulate;
} InputManager;

typedef struct ActionManager {
    ActionMap* actions;
    int action_count;
    int player_count;
} ActionManager;

extern InputManager input_manager;
extern ActionManager action_manager;
extern bool gamepad_connect_events_init;

void gamepad_update(Gamepad* gamepad);
int gamepad_open(SDL_JoystickID id);

#endif