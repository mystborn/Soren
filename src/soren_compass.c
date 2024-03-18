#include <soren_compass.h>
#include <soren_enum_parser.h>

void compass_direction_enum_init(void) {
    soren_enum_parser_add("compassdirection.none", COMPASS_DIRECTION_NONE);
    soren_enum_parser_add("compassdirection.east", COMPASS_DIRECTION_EAST);
    soren_enum_parser_add("compassdirection.north", COMPASS_DIRECTION_NORTH);
    soren_enum_parser_add("compassdirection.west", COMPASS_DIRECTION_WEST);
    soren_enum_parser_add("compassdirection.south", COMPASS_DIRECTION_SOUTH);
    soren_enum_parser_add("compassdirection.northeast", COMPASS_DIRECTION_NORTH_EAST);
    soren_enum_parser_add("compassdirection.north_east", COMPASS_DIRECTION_NORTH_EAST);
    soren_enum_parser_add("compassdirection.northwest", COMPASS_DIRECTION_NORTH_WEST);
    soren_enum_parser_add("compassdirection.north_west", COMPASS_DIRECTION_NORTH_WEST);
    soren_enum_parser_add("compassdirection.southeast", COMPASS_DIRECTION_SOUTH_EAST);
    soren_enum_parser_add("compassdirection.south_east", COMPASS_DIRECTION_SOUTH_EAST);
    soren_enum_parser_add("compassdirection.southwest", COMPASS_DIRECTION_SOUTH_WEST);
    soren_enum_parser_add("compassdirection.south_west", COMPASS_DIRECTION_SOUTH_WEST);
}

SOREN_EXPORT CompassDirection compass_direction_parse(String* str) {
    if (!string_starts_with(str, "compassdirection.")) {
        String prefixed_name = string_create("compassdirection.");
        string_append(&prefixed_name, str);
        int64_t value = soren_parse_enum(string_data(&prefixed_name));
        string_free_resources(&prefixed_name);
        return value;
    } else {
        return soren_parse_enum(string_data(str));
    }
}

SOREN_EXPORT bool compass_direction_try_parse(String* str, CompassDirection* out_direction) {
    int64_t value;
    bool result;

    if (!string_starts_with(str, "compassdirection.")) {
        String prefixed_name = string_create("compassdirection.");
        string_append(&prefixed_name, str);
        result = soren_try_parse_enum(string_data(&prefixed_name), &value);
        string_free_resources(&prefixed_name);
        return value;
    } else {
        result = soren_try_parse_enum(string_data(str), &value);
    }

    if (result && out_direction) {
        *out_direction = value;
    }

    return result;
}

SOREN_EXPORT String* compass_direction_to_string(CompassDirection direction, String* str, bool append_prefix) {
    if (!str) {
        str = string_create_ref("");
    }

    if (append_prefix) {
        string_append(str, "CompassDirection.");
    }

    switch (direction) {
        case COMPASS_DIRECTION_NONE:
            string_append(str, "None");
            break;
        case COMPASS_DIRECTION_EAST:
            string_append(str, "East");
            break;
        case COMPASS_DIRECTION_NORTH_EAST:
            string_append(str, "NorthEast");
            break;
        case COMPASS_DIRECTION_NORTH:
            string_append(str, "North");
            break;
        case COMPASS_DIRECTION_NORTH_WEST:
            string_append(str, "NorthWest");
            break;
        case COMPASS_DIRECTION_WEST:
            string_append(str, "West");
            break;
        case COMPASS_DIRECTION_SOUTH_WEST:
            string_append(str, "SouthWest");
            break;
        case COMPASS_DIRECTION_SOUTH:
            string_append(str, "South");
            break;
        case COMPASS_DIRECTION_SOUTH_EAST:
            string_append(str, "SouthEast");
            break;
        default: break;
    }

    return str;
}