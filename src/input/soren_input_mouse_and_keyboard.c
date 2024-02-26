#include "soren_input_shared.h"

SOREN_EXPORT bool key_check(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 1;
}

SOREN_EXPORT bool key_check_pressed(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 1
        && input_manager.keyboard_previous[key] == 0;
}

SOREN_EXPORT bool key_check_released(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 0
        && input_manager.keyboard_previous[key] == 1;
}

SOREN_EXPORT bool mouse_check(MouseButtons button) {
    return (input_manager.mouse_current & button) == button;
}

SOREN_EXPORT bool mouse_check_pressed(MouseButtons button) {
    return ((input_manager.mouse_current & button) == button)
        && ((input_manager.mouse_previous & button) != button);
}

SOREN_EXPORT bool mouse_check_released(MouseButtons button) {
    return ((input_manager.mouse_current & button) != button) 
        && ((input_manager.mouse_previous & button) == button);
}

SOREN_EXPORT bool mouse_moved(void) {
    return input_manager.mouse_position_current.x != input_manager.mouse_position_previous.x
        || input_manager.mouse_position_current.y != input_manager.mouse_position_previous.y;
}

SOREN_EXPORT Vector mouse_position(void) {
    return input_manager.mouse_position_current;
}