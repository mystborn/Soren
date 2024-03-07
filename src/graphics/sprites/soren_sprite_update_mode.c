#include <graphics/soren_sprite.h>
#include <soren_enum_parser.h>
#include "../../soren_init.h"

void sprite_update_mode_enum_init(void) {
    soren_enum_parser_add("spriteupdatemode.none", SPRITE_UPDATE_NONE);
    soren_enum_parser_add("spriteupdatemode.once", SPRITE_UPDATE_ONCE);
    soren_enum_parser_add("spriteupdatemode.once_stay", SPRITE_UPDATE_ONCE_STAY);
    soren_enum_parser_add("spriteupdatemode.oncestay", SPRITE_UPDATE_ONCE_STAY);
    soren_enum_parser_add("spriteupdatemode.ping_pong", SPRITE_UPDATE_PING_PONG);
    soren_enum_parser_add("spriteupdatemode.pingpong", SPRITE_UPDATE_PING_PONG);
    soren_enum_parser_add("spriteupdatemode.cycle", SPRITE_UPDATE_CYCLE);
}

SOREN_EXPORT SpriteUpdateMode sprite_update_mode_parse(String* str) {
    if (!string_starts_with(str, "spriteupdatemode.")) {
        String prefixed_name = string_create("spriteupdatemode.");
        string_append(&prefixed_name, str);
        int64_t value = soren_parse_enum(string_data(&prefixed_name));
        string_free_resources(&prefixed_name);
        return value;
    } else {
        return soren_parse_enum(string_data(str));
    }
}

SOREN_EXPORT bool sprite_update_mode_try_parse(String* str, SpriteUpdateMode* out_mode) {
    int64_t value;
    bool result;


    if (!string_starts_with(str, "spriteupdatemode.")) {
        String prefixed_name = string_create("spriteupdatemode.");
        string_append(&prefixed_name, str);
        result = soren_try_parse_enum(string_data(&prefixed_name), &value);
        string_free_resources(&prefixed_name);
    } else {
        result = soren_try_parse_enum(string_data(str), &value);
    }

    if (result && out_mode) {
        *out_mode = (SpriteUpdateMode)value;
    }

    return result;
}

SOREN_EXPORT String* sprite_update_mode_to_string(SpriteUpdateMode mode, String* str, bool add_prefix) {
    if (!str) {
        str = string_create_ref("");
    }

    if (add_prefix) {
        string_append(str, "SpriteUpdateMode.");
    }

    switch (mode) {
        case SPRITE_UPDATE_NONE:
            string_append(str, "None");
            break;
        case SPRITE_UPDATE_ONCE:
            string_append(str, "Once");
            break;
        case SPRITE_UPDATE_ONCE_STAY:
            string_append(str, "OnceStay");
            break;
        case SPRITE_UPDATE_PING_PONG:
            string_append(str, "PingPong");
            break;
        case SPRITE_UPDATE_CYCLE:
            string_append(str, "Cycle");
            break;
    }

    return str;
}