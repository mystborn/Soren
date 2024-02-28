#include <graphics/soren_graphics.h>

static bool set_blend_from_color = true;

SOREN_EXPORT void graphics_set_blend_mode_from_color(bool blend_from_color) {
    set_blend_from_color = blend_from_color;
}

SOREN_EXPORT void graphics_set_color(SDL_Renderer* renderer, SDL_Color color) {
    graphics_set_color_rgba(renderer, color.r, color.g, color.b, color.a);
}

SOREN_EXPORT void graphics_set_color_rgba(SDL_Renderer* renderer, int8_t r,  int8_t g, int8_t b, int8_t a) {
    if (set_blend_from_color) {
        SDL_SetRenderDrawBlendMode(renderer, a == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}