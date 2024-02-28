#ifndef SOREN_GRAPHICS_SOREN_CAMERA_H
#define SOREN_GRAPHICS_SOREN_CAMERA_H

#include "../soren_std.h"
#include "../soren_math.h"

typedef struct Camera {
    Rect world_bounds;
    Rect screen_bounds;
    float rotation;
    SDL_Texture* render_target;
} Camera;

#endif