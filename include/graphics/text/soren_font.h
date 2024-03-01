#ifndef SOREN_GRAPHICS_TEXT_SOREN_FONT_H
#define SOREN_GRAPHICS_TEXT_SOREN_FONT_H

#include "../../soren_std.h"
#include "../../soren_math.h"

#include <SDL3_ttf/SDL_ttf.h>

typedef enum FontInterfaceType {
    FONT_INTERFACE_TTF,
    FONT_INTERFACE_BITMAP,
    FONT_INTERFACE_CUSTOM
} FontInterfaceType;

typedef struct FontInterfaceMethods {
    Vector (*measure)(void* context, SDL_Renderer* renderer, const char* text, int count);
    void (*draw)(void* context, SDL_Renderer* renderer, const char* text, int count, Vector position, SDL_FColor color);
    void (*draw_ext)(void* context, SDL_Renderer* renderer, const char* text, int count, Vector position, SDL_FColor color, float rotation, Vector origin, Vector scale, SDL_FlipMode flip, bool rtl);
    float (*line_height)(void* context);
    float (*letter_spacing)(void* context);
    void (*set_letter_spacing)(void* context, float letter_spacing);
    void (*free)(void* context);
} FontInterfaceMethods;

typedef struct FontInterface {
    FontInterfaceType type;
    void* context;
    FontInterfaceMethods* methods;
} FontInterface;

SOREN_EXPORT FontInterface* font_create_ttf(TTF_Font* font, bool pass_ownership);
SOREN_EXPORT FontInterface* font_create_custom(void* context, FontInterfaceMethods* vtable);

SOREN_EXPORT void font_init_ttf(FontInterface* font, TTF_Font* ttf, bool pass_ownership);
SOREN_EXPORT void font_init_custom(FontInterface* font, void* context, FontInterfaceMethods* vtable);

SOREN_EXPORT void font_free_resources(FontInterface* font);
SOREN_EXPORT void font_free(FontInterface* font);

SOREN_EXPORT float font_line_height(FontInterface* font);
SOREN_EXPORT float font_letter_spacing(FontInterface* font);
SOREN_EXPORT void font_set_letter_spacing(FontInterface* font, float letter_spacing);

SOREN_EXPORT Vector font_measure(FontInterface* font, SDL_Renderer* renderer, const char* str, int count);

SOREN_EXPORT void font_draw(FontInterface* font, SDL_Renderer* renderer, const char* str, int count, Vector position, SDL_FColor color);

SOREN_EXPORT void font_draw_ext(
    FontInterface* font,
    SDL_Renderer* renderer,
    const char* str,
    int count,
    Vector position,
    SDL_FColor color,
    float rotation,
    Vector origin,
    Vector scale,
    SDL_FlipMode flip,
    bool rtl);

#endif