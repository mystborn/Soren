#include "soren_input_shared.h"

#include <generic_iterators/list_iterator.h>

bool gamepad_connect_events_init = false;

static inline bool gamepad_check_impl(int button, Gamepad* gamepad) {
    return (GAMEPAD_BUTTON(button) & gamepad->button_current) != 0;
}

static inline bool gamepad_check_pressed_impl(int button, Gamepad* gamepad) {
    return (GAMEPAD_BUTTON(button) & gamepad->button_current) != 0
        && (GAMEPAD_BUTTON(button) & gamepad->button_previous) == 0;
}

static inline bool gamepad_check_released_impl(int button, Gamepad* gamepad) {
    return (GAMEPAD_BUTTON(button) & gamepad->button_current) == 0
        && (GAMEPAD_BUTTON(button) & gamepad->button_previous) != 0;
}

SOREN_EXPORT bool gamepad_check_index(int button, int controller) {
    if (controller == GAMEPAD_CHECK_ALL) {
        int index;
        list_iter_start(input_manager.connected_gamepads, index) {
            if (gamepad_check_impl(button, &input_manager.gamepads[index])) {
                return true;
            }
        }
        list_iter_end;
        return false;
    } else if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return false;
    }

    return gamepad_check_impl(button, &input_manager.gamepads[controller]);
}

SOREN_EXPORT bool gamepad_check_pressed_index(int button, int controller) {
    if (controller == GAMEPAD_CHECK_ALL) {
        int index;
        list_iter_start(input_manager.connected_gamepads, index) {
            if (gamepad_check_pressed_impl(button, &input_manager.gamepads[index])) {
                return true;
            }
        }
        list_iter_end;
        return false;
    } else if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return false;
    }

    return gamepad_check_pressed_impl(button, &input_manager.gamepads[controller]);
}

SOREN_EXPORT bool gamepad_check_released_index(int button, int controller) {
    if (controller == GAMEPAD_CHECK_ALL) {
        int index;
        list_iter_start(input_manager.connected_gamepads, index) {
            if (gamepad_check_released_impl(button, &input_manager.gamepads[index])) {
                return true;
            }
        }
        list_iter_end;
        return false;
    } else if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return false;
    }

    return gamepad_check_released_impl(button, &input_manager.gamepads[controller]);
}

static inline bool gamepad_axis_outside_deadzone(SDL_GamepadAxis axis, Gamepad* gamepad, int16_t value) {
    switch(axis) {
        case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
        case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
            return true;
        case SDL_GAMEPAD_AXIS_LEFTX:
        case SDL_GAMEPAD_AXIS_LEFTY:
            if (value < -gamepad->left_deadzone || value > gamepad->left_deadzone)
                return true;
            break;
        case SDL_GAMEPAD_AXIS_RIGHTX:
        case SDL_GAMEPAD_AXIS_RIGHTY:
            if (value < -gamepad->right_deadzone || value > gamepad->right_deadzone)
                return true;
            break;
    }

    return false;
}

SOREN_EXPORT int16_t gamepad_axis_value_index(SDL_GamepadAxis axis, int controller) {
    if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return 0;
    }

    Gamepad* gamepad = input_manager.gamepads + controller;

    int16_t value = SDL_GetGamepadAxis(gamepad->gamepad, axis);
    if (gamepad_axis_outside_deadzone(axis, gamepad, value)) {
        return value;
    }

    return 0;
}

SOREN_EXPORT int16_t gamepad_stick_get_deadzone(SDL_GamepadButton stick, int controller) {
    if (stick != SDL_GAMEPAD_BUTTON_LEFT_STICK
        && stick != SDL_GAMEPAD_BUTTON_RIGHT_STICK)
    {
        throw(IllegalArgumentException, "Expected SDL_GAMEPAD_BUTTON_LEFT_STICK or "
            "SDL_GAMEPAD_BUTTON_RIGHT_STICK");
    }


    if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return 0;
    }

    if (stick == SDL_GAMEPAD_BUTTON_LEFT_STICK) {
        return input_manager.gamepads[controller].left_deadzone;
    } else {
        return input_manager.gamepads[controller].right_deadzone;
    }
}

SOREN_EXPORT void gamepad_stick_set_deadzone(SDL_GamepadButton stick, int16_t deadzone, int controller) {
    if (stick != SDL_GAMEPAD_BUTTON_LEFT_STICK
        && stick != SDL_GAMEPAD_BUTTON_RIGHT_STICK)
    {
        throw(IllegalArgumentException, "Expected SDL_GAMEPAD_BUTTON_LEFT_STICK or "
            "SDL_GAMEPAD_BUTTON_RIGHT_STICK");
    }


    if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return;
    }

    if (stick == SDL_GAMEPAD_BUTTON_LEFT_STICK) {
        input_manager.gamepads[controller].left_deadzone = deadzone;
    } else {
        input_manager.gamepads[controller].right_deadzone = deadzone;
    }
}

SOREN_EXPORT SDL_Gamepad* gamepad_get_raw(int controller) {
    if (controller == GAMEPAD_CHECK_FIRST) {
        controller = input_manager.connected_gamepads->count != 0
            ? int_list_get(input_manager.connected_gamepads, 0)
            : -1;
    }

    if (controller < 0 
        || controller >= MAX_GAMEPADS
        || !input_manager.gamepads[controller].active)
    {
        return 0;
    }

    return input_manager.gamepads[controller].gamepad;
}

SOREN_EXPORT GamepadConnectionEvent* gamepad_connected_event(void) {
    if (gamepad_connect_events_init) {
        gamepad_connection_event_init(&input_manager.gamepad_connected);
        gamepad_connection_event_init(&input_manager.gamepad_disconnected);
    }

    return &input_manager.gamepad_connected;
}

SOREN_EXPORT GamepadConnectionEvent* gamepad_disconnected_event(void) {
    if (gamepad_connect_events_init) {
        gamepad_connection_event_init(&input_manager.gamepad_connected);
        gamepad_connection_event_init(&input_manager.gamepad_disconnected);
    }

    return &input_manager.gamepad_disconnected;
}

void gamepad_update(Gamepad* gamepad) {
    gamepad->button_previous = gamepad->button_current;
    gamepad->button_current = 0;

    for (int i = SDL_GAMEPAD_BUTTON_SOUTH; i < SDL_GAMEPAD_BUTTON_MAX; i++) {
        if (SDL_GetGamepadButton(gamepad->gamepad, i)) {
            gamepad->button_current |= GAMEPAD_BUTTON(i);
        }
    }

    for (int i = SDL_GAMEPAD_AXIS_LEFTX; i < SDL_GAMEPAD_AXIS_MAX; i++) {
        int16_t axis = SDL_GetGamepadAxis(gamepad->gamepad, i);
        if (!gamepad_axis_outside_deadzone(i, gamepad, axis)) {
            continue;
        }

        int index;
        switch(i) {
            case SDL_GAMEPAD_AXIS_LEFTX:
                index = axis < 0 ? SDL_GAMEPAD_BUTTON_LEFTSTICKLEFT : SDL_GAMEPAD_BUTTON_LEFTSTICKRIGHT;
                break;
            case SDL_GAMEPAD_AXIS_LEFTY:
                index = axis < 0 ? SDL_GAMEPAD_BUTTON_LEFTSTICKUP : SDL_GAMEPAD_BUTTON_LEFTSTICKDOWN;
                break;
            case SDL_GAMEPAD_AXIS_RIGHTX:
                index = axis < 0 ? SDL_GAMEPAD_BUTTON_RIGHTSTICKRIGHT : SDL_GAMEPAD_BUTTON_RIGHTSTICKRIGHT;
                break;
            case SDL_GAMEPAD_AXIS_RIGHTY:
                index = axis < 0 ? SDL_GAMEPAD_BUTTON_RIGHTSTICKUP : SDL_GAMEPAD_BUTTON_RIGHTSTICKDOWN;
                break;
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                index = SDL_GAMEPAD_BUTTON_LEFTTRIGGER;
                break;
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                index = SDL_GAMEPAD_BUTTON_RIGHTTRIGGER;
                break;
        }

        gamepad->button_current |= GAMEPAD_BUTTON(index);
    }
}

int gamepad_open(SDL_JoystickID id) {
    SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
    if (!gamepad)
        return -1;

    int index = ecs_dispenser_get(&input_manager.gamepad_index_dispenser);
    int_list_add(input_manager.connected_gamepads, index);
    Gamepad* controller = input_manager.gamepads + index;
    controller->id = id;
    controller->gamepad = gamepad;
    controller->active = true;
    gamepad_update(controller);
    return index;
}