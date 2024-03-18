#ifndef SOREN_COMPASS_DIRECTIONS_H
#define SOREN_COMPASS_DIRECTIONS_H

#include "soren_std.h"
#include "soren_math.h"

typedef enum CompassDirection {
    COMPASS_DIRECTION_NONE = 0,
    COMPASS_DIRECTION_NORTH = 1,
    COMPASS_DIRECTION_EAST = 2,
    COMPASS_DIRECTION_SOUTH = 4,
    COMPASS_DIRECTION_WEST = 8,
    COMPASS_DIRECTION_NORTH_EAST = COMPASS_DIRECTION_NORTH | COMPASS_DIRECTION_EAST,
    COMPASS_DIRECTION_NORTH_WEST = COMPASS_DIRECTION_NORTH | COMPASS_DIRECTION_WEST,
    COMPASS_DIRECTION_SOUTH_EAST = COMPASS_DIRECTION_SOUTH | COMPASS_DIRECTION_EAST,
    COMPASS_DIRECTION_SOUTH_WEST = COMPASS_DIRECTION_SOUTH | COMPASS_DIRECTION_WEST
} CompassDirection;

SOREN_EXPORT CompassDirection compass_direction_parse(String* str);
SOREN_EXPORT bool compass_direction_try_parse(String* str, CompassDirection* out_mode);
SOREN_EXPORT String* compass_direction_to_string(CompassDirection mode, String* str, bool append_prefix);

static inline float compass_direction_to_degrees(CompassDirection direction) {
    switch (direction) {
        case COMPASS_DIRECTION_EAST: return 0;
        case COMPASS_DIRECTION_NORTH_EAST: return 45;
        case COMPASS_DIRECTION_NORTH: return 90;
        case COMPASS_DIRECTION_NORTH_WEST: return 135;
        case COMPASS_DIRECTION_WEST: return 180;
        case COMPASS_DIRECTION_SOUTH_WEST: return 225;
        case COMPASS_DIRECTION_SOUTH: return 270;
        case COMPASS_DIRECTION_SOUTH_EAST: return 315;
        default: return 0;
    }
}

static inline float compass_direction_to_radians(CompassDirection directions) {
    return degrees_to_radians(compass_direction_to_degrees(directions));
}

static inline Vector compass_direction_to_vector(CompassDirection direction) {
    float x = 0;
    float y = 0;
    switch (direction) {
        case COMPASS_DIRECTION_EAST:
            x = 1;
            break;
        case COMPASS_DIRECTION_NORTH_EAST:
            x = 0.70710677f;
            y = -0.70710677f;
            break;
        case COMPASS_DIRECTION_NORTH:
            y = -1;
            break;
        case COMPASS_DIRECTION_NORTH_WEST:
            x = -0.70710677f;
            y = -0.70710677f;
            break;
        case COMPASS_DIRECTION_WEST:
            x = -1;
            break;
        case COMPASS_DIRECTION_SOUTH_WEST:
            x = -0.70710677f;
            y = 0.70710677f;
            break;
        case COMPASS_DIRECTION_SOUTH:
            y = 1;
            break;
        case COMPASS_DIRECTION_SOUTH_EAST:
            x = 0.70710677f;
            y = 0.70710677f;
            break;
        default:
            break;
    }

    return vector_create(x, y);
}

#endif