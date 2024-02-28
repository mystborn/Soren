#ifndef SOREN_GRAPHICS_SOREN_GRAPHICS_H
#define SOREN_GRAPHICS_SOREN_GRAPHICS_H

#include "soren_std.h"

#include <SDL3/SDL.h>

SOREN_EXPORT void graphics_set_blend_mode_from_color(bool blend_from_color);
SOREN_EXPORT void graphics_set_color(SDL_Renderer* renderer, SDL_Color color);
SOREN_EXPORT void graphics_set_color_rgba(SDL_Renderer* renderer, int8_t r,  int8_t g, int8_t b, int8_t a);

#endif