#include "soren_font_shared.h"

SOREN_EXPORT FontInterface* font_create_ttf(TTF_Font* font, bool pass_ownership) {
    FontInterface* interface = soren_malloc(sizeof(*interface));
    font_init_ttf(interface, font, pass_ownership);

    return interface;
}

SOREN_EXPORT FontInterface* font_create_custom(void* context, FontInterfaceMethods* vtable) {
    FontInterface* interface = soren_malloc(sizeof(*interface));
    font_init_custom(interface, context, vtable);

    return interface;
}

SOREN_EXPORT void font_init_ttf(FontInterface* font, TTF_Font* ttf, bool pass_ownership) {
    font->methods = NULL;
    font->type = FONT_INTERFACE_TTF;
    font->context = font_ttf_create(ttf, pass_ownership);
}

SOREN_EXPORT void font_init_custom(FontInterface* font, void* context, FontInterfaceMethods* vtable) {
    font->type = FONT_INTERFACE_CUSTOM;
    font->methods = vtable;
    font->context = context;
}

SOREN_EXPORT void font_free_resources(FontInterface* font) {
    if (!font) {
        return;
    }

    switch (font->type) {
        case FONT_INTERFACE_TTF:
            font_ttf_free((FontImplTtf*)font->context);
            break;
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->free) {
                return;
            }
            font->methods->free(font->context);
            break;
    }
}

SOREN_EXPORT void font_free(FontInterface* font) {
    font_free_resources(font);
    soren_free(font);
}

SOREN_EXPORT float font_line_height(FontInterface* font) {
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            return font_ttf_line_height((FontImplTtf*)font->context);
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->line_height) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            return font->methods->line_height(font->context);
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }

    return 0;
}

SOREN_EXPORT float font_letter_spacing(FontInterface* font) {
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            return font_ttf_letter_spacing((FontImplTtf*)font->context);
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->letter_spacing) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            return font->methods->letter_spacing(font->context);
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }

    return 0;
}

SOREN_EXPORT void font_set_letter_spacing(FontInterface* font, float letter_spacing) {
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            font_ttf_set_letter_spacing((FontImplTtf*)font->context, letter_spacing);
            break;
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->set_letter_spacing) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            font->methods->set_letter_spacing(font->context, letter_spacing);
            break;
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }
}

SOREN_EXPORT Vector font_measure(FontInterface* font, SDL_Renderer* renderer, const char* str, int count) {
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            return font_ttf_measure((FontImplTtf*)font->context, renderer, str, count);
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->measure) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            return font->methods->measure(font->context, renderer, str, count);
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }

    return VECTOR_ZERO;
}

SOREN_EXPORT void font_draw(FontInterface* font, SDL_Renderer* renderer, const char* str, int count, Vector position, SDL_Color color) {
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            font_ttf_draw((FontImplTtf*)font->context, renderer, str, count, position, color);
            break;
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->draw) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            font->methods->draw(font->context, renderer, str, count, position, color);
            break;
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }
}

SOREN_EXPORT void font_draw_ext(
    FontInterface* font,
    SDL_Renderer* renderer,
    const char* str,
    int count,
    Vector position,
    SDL_Color color,
    float rotation,
    Vector origin,
    Vector scale,
    SDL_FlipMode flip,
    bool rtl)
{
    switch (font->type) {
        case FONT_INTERFACE_TTF:
            font_ttf_draw_ext((FontImplTtf*)font->context, renderer, str, count, position, color, rotation, origin, scale, flip, rtl);
            break;
        case FONT_INTERFACE_CUSTOM:
            if (!font->methods || !font->methods->draw_ext) {
                throw(NotImplementedException, "FontInterface does not implement " __FUNCTION__);
            }
            font->methods->draw_ext(font->context, renderer, str, count, position, color, rotation, origin, scale, flip, rtl);
            break;
        default:
            throw(NotImplementedException, __FUNCTION__ " not implemented");
            break;
    }
}