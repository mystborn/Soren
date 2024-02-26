#include "soren_input_shared.h"

#include <generic_iterators/list_iterator.h>

static bool action_manager_initialized = false;
ActionManager action_manager = {0};

static inline ActionMap* action_manager_get_map(int action, int player) {
    soren_assert(player < action_manager.player_count);
    soren_assert(action < action_manager.action_count);

    return action_manager.actions + action + player * action_manager.action_count;
}

void action_map_add_key(int action, int player, SDL_Scancode key) {
    ActionMap* map = action_manager_get_map(action, player);
    scancode_list_add(&map->keys, key);
}

void action_map_add_gamepad_button(int action, int player, uint32_t button) {
    ActionMap* map = action_manager_get_map(action, player);
    GamepadAction gamepad_action = (GamepadAction){ .gamepad = GAMEPAD_CHECK_ALL, .button = button };
    gamepad_action_list_add(&map->buttons, gamepad_action);
}

void action_map_add_gamepad_button_ext(int action, int player, uint32_t button, int controller_index) {
    ActionMap* map = action_manager_get_map(action, player);
    GamepadAction gamepad_action = (GamepadAction){ .gamepad = controller_index, .button = button };
    gamepad_action_list_add(&map->buttons, gamepad_action);
}

void action_map_add_mouse_button(int action, int player, MouseButtons button) {
    ActionMap* map = action_manager_get_map(action, player);
    mouse_button_list_add(&map->mouse_buttons, button);
}

bool action_map_remove_key(int action, int player, SDL_Scancode key) {
    ActionMap* map = action_manager_get_map(action, player);
    for(int i = 0; i < map->keys.count; i++) {
        if (map->keys.buffer[i] == key) {
            scancode_list_remove(&map->keys, i);
            return true;
        }
    }

    return false;
}

bool action_map_remove_gamepad_button(int action, int player, uint32_t button) {
    ActionMap* map = action_manager_get_map(action, player);
    for(int i = 0; i < map->buttons.count; i++) {
        if (map->buttons.buffer[i].button == button) {
            gamepad_action_list_remove(&map->buttons, i);
            return true;
        }
    }

    return false;
}

bool action_map_remove_gamepad_button_ext(int action, int player, uint32_t button, int controller_index) {
    ActionMap* map = action_manager_get_map(action, player);

    GamepadAction gamepad_action;
    list_iter_start(&map->buttons, gamepad_action) {
        if (gamepad_action.button == button && gamepad_action.gamepad == controller_index) {
            gamepad_action_list_remove(&map->buttons, list_iter_i);
        }
    }
    list_iter_end

    return false;
}

bool action_map_remove_mouse_button(int action, int player, MouseButtons button) {
    ActionMap* map = action_manager_get_map(action, player);
    for(int i = 0; i < map->keys.count; i++) {
        if (map->mouse_buttons.buffer[i] == button) {
            mouse_button_list_remove(&map->mouse_buttons, i);
            return true;
        }
    }

    return false;
}

void action_map_clear(int action, int player) {
    ActionMap* map = action_manager_get_map(action, player);
    scancode_list_clear(&map->keys);
    gamepad_action_list_clear(&map->buttons);
    mouse_button_list_clear(&map->mouse_buttons);
}

bool action_check_impl(int action, int player) {
    ActionMap* map = action_manager_get_map(action, player);
    return map->current_pressed;
}

bool action_check_pressed_impl(int action, int player) {
    ActionMap* map = action_manager_get_map(action, player);
    return map->current_pressed && !map->previous_pressed;
}

bool action_check_released_impl(int action, int player) {
    ActionMap* map = action_manager_get_map(action, player);
    return !map->current_pressed && map->previous_pressed;
}

static void action_map_update(ActionMap* map) {
    SDL_Scancode key;
    GamepadAction gp_action;
    MouseButtons mouse_button;

    map->previous_pressed = map->current_pressed;

    list_iter_start(&map->keys, key) {
        if (key_check(key)) {
            map->current_pressed = true;
            return;
        }
    }
    list_iter_end

    list_iter_start(&map->buttons, gp_action) {
        if (gamepad_check(gp_action.button, gp_action.gamepad)) {
            map->current_pressed = true;
            return;
        }
    }
    list_iter_end

    list_iter_start(&map->mouse_buttons, mouse_button) {
        if (mouse_check(mouse_button)) {
            map->current_pressed = true;
            return;
        }
    }
    list_iter_end

    map->current_pressed = false;
}

void action_manager_update(void) {
    int count = action_manager.action_count * action_manager.player_count;
    for (int i = 0; i < count; i++) {
        action_map_update(action_manager.actions + i);
    }
}

static bool action_map_init(ActionMap* map) {
    bool keys = scancode_list_init(&map->keys);
    bool buttons = gamepad_action_list_init(&map->buttons);
    bool mouse_buttons = mouse_button_list_init(&map->mouse_buttons);

    if (keys && buttons && mouse_buttons) {
        return true;
    }

    if (keys) {
        scancode_list_free_resources(&map->keys);
    }

    if (buttons) {
        gamepad_action_list_free_resources(&map->buttons);
    }

    if (mouse_buttons) {
        mouse_button_list_free_resources(&map->mouse_buttons);
    }

    return false;
}

static void action_map_free_resources(ActionMap* map) {
    scancode_list_free_resources(&map->keys);
    gamepad_action_list_free_resources(&map->buttons);
    mouse_button_list_free_resources(&map->mouse_buttons);
}

bool action_manager_init(int action_count, int player_count) {
    int total = action_count * player_count;
    action_manager.actions = soren_malloc(total * sizeof(*action_manager.actions));
    if (!action_manager.actions) {
        return false;
    }

    for (int i = 0; i < total; i++) {
        if(!action_map_init(action_manager.actions + i)) {
            for (int j = 0; j < i; j++) {
                action_map_free_resources(action_manager.actions + j);
            }
            soren_free(action_manager.actions);
            return false;
        }
    }

    return true;
}