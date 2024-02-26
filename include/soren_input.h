#ifndef SOREN_INPUT_H
#define SOREN_INPUT_H

#include <soren_std.h>

#include <stdint.h>

#include <SDL3/SDL.h>
#include <generic_list.h>
#include <generic_event.h>

#include "soren_math.h"

#define MAX_GAMEPADS 16

#define GAMEPAD_CHECK_ALL -1
#define GAMEPAD_CHECK_FIRST -2

typedef enum MouseButtons {
    MOUSE_BUTTON_LEFT = SDL_BUTTON_LMASK,
    MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MMASK,
    MOUSE_BUTTON_RIGHT = SDL_BUTTON_RMASK,
    MOUSE_BUTTON_X1 = SDL_BUTTON_X1MASK,
    MOUSE_BUTTON_X2 = SDL_BUTTON_X2MASK,
    MOUSE_BUTTON_WHEEL_UP = SDL_BUTTON(6),
    MOUSE_BUTTON_WHEEL_DOWN = SDL_BUTTON(7),
    MOUSE_BUTTON_WHEEL_LEFT = SDL_BUTTON(8),
    MOUSE_BUTTON_WHEEL_RIGHT = SDL_BUTTON(9)
} MouseButtons;

enum GamepadButtonExtended {
    SDL_GAMEPAD_BUTTON_LEFTSTICKUP = SDL_GAMEPAD_BUTTON_MAX,
    SDL_GAMEPAD_BUTTON_LEFTSTICKLEFT,
    SDL_GAMEPAD_BUTTON_LEFTSTICKDOWN,
    SDL_GAMEPAD_BUTTON_LEFTSTICKRIGHT,
    SDL_GAMEPAD_BUTTON_RIGHTSTICKUP,
    SDL_GAMEPAD_BUTTON_RIGHTSTICKLEFT,
    SDL_GAMEPAD_BUTTON_RIGHTSTICKDOWN,
    SDL_GAMEPAD_BUTTON_RIGHTSTICKRIGHT,
    SDL_GAMEPAD_BUTTON_LEFTTRIGGER,
    SDL_GAMEPAD_BUTTON_RIGHTTRIGGER,
    SDL_GAMEPAD_BUTTON_EXTENSION_MAX
};

typedef struct GamepadAction {
    uint32_t button;
    int gamepad;
} GamepadAction;

EVENT_DEFINE_1_H(GamepadConnectionEvent, gamepad_connection_event, int)

SOREN_EXPORT bool key_check(SDL_Scancode key);
SOREN_EXPORT bool key_check_pressed(SDL_Scancode key);
SOREN_EXPORT bool key_check_released(SDL_Scancode key);

SOREN_EXPORT bool mouse_check(MouseButtons button);
SOREN_EXPORT Vector mouse_position(void);
SOREN_EXPORT bool mouse_check_pressed(MouseButtons button);
SOREN_EXPORT bool mouse_check_released(MouseButtons button);
SOREN_EXPORT bool mouse_moved(void);

SOREN_EXPORT bool gamepad_check_index(int button, int controller);
SOREN_EXPORT bool gamepad_check_pressed_index(int button, int controller);
SOREN_EXPORT bool gamepad_check_released_index(int button, int controller);

SOREN_EXPORT int16_t gamepad_axis_value_index(SDL_GamepadAxis axis, int controller);
SOREN_EXPORT int16_t gamepad_stick_get_deadzone(SDL_GamepadButton stick, int controller);
SOREN_EXPORT void gamepad_stick_set_deadzone(SDL_GamepadButton stick, int16_t deadzone, int controller);
SOREN_EXPORT SDL_Gamepad* gamepad_get_raw(int controller);

SOREN_EXPORT GamepadConnectionEvent* gamepad_connected_event(void);
SOREN_EXPORT GamepadConnectionEvent* gamepad_disconnected_event(void);

SOREN_EXPORT bool input_manager_init(void);
SOREN_EXPORT void input_manager_update(void);
SOREN_EXPORT void input_manager_event(SDL_Event* event);
SOREN_EXPORT void input_manager_free(void);

SOREN_EXPORT bool action_manager_init(int action_count, int player_count);
SOREN_EXPORT void action_manager_update(void);
SOREN_EXPORT void action_manager_free(void);

SOREN_EXPORT void action_map_add_key(int action, int player, SDL_Scancode key);
SOREN_EXPORT void action_map_add_gamepad_button(int action, int player, uint32_t button);
SOREN_EXPORT void action_map_add_gamepad_button_ext(int action, int player, uint32_t button, int controller_index);
SOREN_EXPORT void action_map_add_mouse_button(int action, int player, MouseButtons button);
SOREN_EXPORT bool action_map_remove_key(int action, int player, SDL_Scancode key);
SOREN_EXPORT bool action_map_remove_gamepad_button(int action, int player, uint32_t button);
SOREN_EXPORT bool action_map_remove_gamepad_button_ext(int action, int player, uint32_t button, int controller_index);
SOREN_EXPORT bool action_map_remove_mouse_button(int action, int player, MouseButtons button);
SOREN_EXPORT void action_map_clear(int action, int player);

SOREN_EXPORT bool action_check_impl(int action, int player);
SOREN_EXPORT bool action_check_pressed_impl(int action, int player);
SOREN_EXPORT bool action_check_released_impl(int action, int player);

#define Z_SOREN_input_index_selector(arg1, ...) (arg1)

#define gamepad_check(button, ...) \
    gamepad_check_index((button), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) -1))

#define gamepad_check_pressed(button, ...) \
    gamepad_check_pressed_index((button), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) -1))

#define gamepad_check_released(button, ...) \
    gamepad_check_released_index((button), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) -1))

#define action_check(action, ...) \
    action_check_impl((action), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) 0))

#define action_check_pressed(action, ...) \
    action_check_pressed_impl((action), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) 0))

#define action_check_released(action, ...) \
    action_check_released_impl((action), Z_SOREN_input_index_selector(__VA_ARGS__ __VA_OPT__(,) 0))

#endif