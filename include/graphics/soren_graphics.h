#ifndef SOREN_GRAPHICS_SOREN_GRAPHICS_H
#define SOREN_GRAPHICS_SOREN_GRAPHICS_H

#include "../soren_std.h"
#include "soren_camera.h"

#include <SDL3/SDL.h>

#define COLOR_DECONSTRUCT(color) color.r, color.g, color.b, color.a
// #define COLOR_CONSTRUCT(r, g, b, a) (SDL_Color){ r, g, b, a }

#define COLOR_CONSTRUCT_BYTES(r, g, b, a) (SDL_FColor){ (r) / 255.f, (g) / 255.f, (b) / 255.f, (a) / 255.f }
#define COLOR_CONSUTRCT_FLOATS(r, g, b, a) (SDL_FColor){ (r), (g), (b), (a) }

#define COLOR_CONSTRUCT(r, g, b, a) \
    _Generic((r), \
        uint8_t: COLOR_CONSTRUCT_BYTES(r, g, b, a), \
        int: COLOR_CONSTRUCT_BYTES(r, g, b, a), \
        float: COLOR_CONSUTRCT_FLOATS(r, g, b, a))

SOREN_EXPORT void graphics_set_camera(Camera* camera);
SOREN_EXPORT Camera* graphics_get_camera(void);

#endif