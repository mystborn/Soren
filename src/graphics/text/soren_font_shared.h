#ifndef SOREN_FONT_SHARED_H
#define SOREN_FONT_SHARED_H

#include <graphics/text/soren_font.h>
#include <soren_generics.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <generic_map.h>
#include <generic_list.h>
#include <sso_string.h>

typedef struct FontImplTtf FontImplTtf;

FontImplTtf* font_ttf_create(TTF_Font* font, bool pass_ownership);
void font_ttf_free(FontImplTtf* font);

float font_ttf_line_height(FontImplTtf* font);
float font_ttf_letter_spacing(FontImplTtf* font);
void font_ttf_set_letter_spacing(FontImplTtf* font, float spacing);

Vector font_ttf_measure(FontImplTtf* font, SDL_Renderer* renderer, const char* str, int count);

void font_ttf_draw(
    FontImplTtf* font,
    SDL_Renderer* renderer,
    const char* str,
    int count,
    Vector position,
    SDL_FColor color);

void font_ttf_draw_ext(
    FontImplTtf* font,
    SDL_Renderer* renderer,
    const char* text,
    int count,
    Vector position,
    SDL_FColor color,
    float rotation,
    Vector origin,
    Vector scale,
    SDL_FlipMode flip,
    bool rtl);

#endif