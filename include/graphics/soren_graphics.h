#ifndef SOREN_GRAPHICS_SOREN_GRAPHICS_H
#define SOREN_GRAPHICS_SOREN_GRAPHICS_H

#include "../soren_std.h"
#include "soren_camera.h"

#include <SDL3/SDL.h>

#define COLOR_DECONSTRUCT(color) color.r, color.g, color.b, color.a
#define COLOR_CONSTRUCT(r, g, b, a) (SDL_Color){ r, g, b, a }

SOREN_EXPORT void graphics_set_camera(Camera* camera);
SOREN_EXPORT Camera* graphics_get_camera(void);

#endif