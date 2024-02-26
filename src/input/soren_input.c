#include "soren_input_shared.h"

#include <generic_iterators/list_iterator.h>

EVENT_DEFINE_C(GamepadConnectionEvent, ganepad_connection_event)

InputManager input_manager = {0};
static bool input_manager_initialized = false;

SOREN_EXPORT bool input_manager_init(void) {
    if (input_manager_initialized) {
        return false;
    }

    if (!SDL_WasInit(SDL_INIT_GAMEPAD)) {
        if (SDL_InitSubSystem(SDL_INIT_GAMEPAD) != 0) {
            return false;
        }
    }

    input_manager.connected_gamepads = int_list_create();
    ecs_dispenser_init(&input_manager.gamepad_index_dispenser);

    input_manager.keyboard_current = SDL_GetKeyboardState(&input_manager.key_count);
    int keyboard_size = input_manager.key_count * sizeof(*input_manager.keyboard_previous);
    input_manager.keyboard_previous = soren_malloc(keyboard_size);
    memcpy(input_manager.keyboard_previous, input_manager.keyboard_current, keyboard_size);

    input_manager.mouse_current = SDL_GetMouseState(&input_manager.mouse_position_current.x, &input_manager.mouse_position_current.y);
    input_manager.mouse_previous = input_manager.mouse_current;

    for (int i = 0; i < MAX_GAMEPADS; i++) {
        input_manager.gamepads[i].left_deadzone = (int16_t)(INT16_MAX * .15f);
        input_manager.gamepads[i].right_deadzone = (int16_t)(INT16_MAX * .15f);
    }

    input_manager_initialized = true;

    return true;
}

SOREN_EXPORT void input_manager_update(void) {
    input_manager.mouse_previous = input_manager.mouse_current;
    input_manager.mouse_position_previous = input_manager.mouse_position_current;

    memcpy(input_manager.keyboard_previous, input_manager.keyboard_current, input_manager.key_count * sizeof(*input_manager.keyboard_previous));

    input_manager.mouse_current = SDL_GetMouseState(&input_manager.mouse_position_current.x, &input_manager.mouse_position_current.y);
    input_manager.keyboard_current = SDL_GetKeyboardState(NULL);

    input_manager.wheel_delta = input_manager.wheel_accumulate;
    input_manager.wheel_total = vector_add(input_manager.wheel_total, input_manager.wheel_accumulate);
    input_manager.wheel_accumulate = VECTOR_ZERO;

    if (input_manager.wheel_delta.x < 0) {
        input_manager.mouse_current |= MOUSE_BUTTON_WHEEL_LEFT;
    } else if (input_manager.wheel_delta.x > 0) {
        input_manager.mouse_current |= MOUSE_BUTTON_WHEEL_RIGHT;
    }

    if (input_manager.wheel_delta.y < 0) {
        input_manager.mouse_current |= MOUSE_BUTTON_WHEEL_UP;
    } else if (input_manager.wheel_delta.y > 0) {
        input_manager.mouse_current |= MOUSE_BUTTON_WHEEL_DOWN;
    }

    int i = 0;
    list_iter_start(input_manager.connected_gamepads, i) {
        gamepad_update(input_manager.gamepads + i);
    }
    list_iter_end
}

SOREN_EXPORT void input_manager_event(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_GAMEPAD_ADDED:
            int controller = gamepad_open(event->gdevice.which);
            if (gamepad_connect_events_init) {
                GamepadConnectionEvent* connect = gamepad_connected_event();
                gamepad_connection_event_trigger(connect, controller);
            }
            break;
        case SDL_EVENT_GAMEPAD_REMOVED:
            int i = 0;
            int index = 0;
            list_iter_start(input_manager.connected_gamepads, i) {
                Gamepad* gamepad = input_manager.gamepads + i; 
                if (gamepad->id == event->gdevice.which) {
                    *gamepad = (Gamepad){0};
                    ecs_dispenser_release(&input_manager.gamepad_index_dispenser, i);
                    int_list_remove(input_manager.connected_gamepads, index);
                    if (gamepad_connect_events_init) {
                        GamepadConnectionEvent* disconnect = gamepad_disconnected_event();
                        gamepad_connection_event_trigger(disconnect, i);
                    }
                    return;
                }

                index++;
            }
            list_iter_end
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            float x = event->wheel.x;
            float y = event->wheel.y;
            if (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                x = -x;
                y = -y;
            }

            input_manager.wheel_accumulate.x += x;
            input_manager.wheel_accumulate.y += y;
    }
}

SOREN_EXPORT void input_manager_free(void) {
    ecs_dispenser_free(&input_manager.gamepad_index_dispenser);
    int_list_free(input_manager.connected_gamepads);
}