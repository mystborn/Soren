#include "soren_font_shared.h"

#include <graphics/soren_graphics.h>
#include <graphics/soren_primitives.h>

#define CHARACTER_REGION_WIDTH 16
#define CHARACTER_REGION_SIZE 256

static soren_thread_local String memo_string = STRING_EMPTY_STATIC;

typedef struct GlyphInfo {
    RectF texture_bounds;
    float left_bearing;
    float right_bearing;
    float width;
} GlyphInfo;

typedef struct FontCharacterRegion {
    Char32 start;
    Char32 end;
    int texture_index;
    GlyphInfo glyphs[CHARACTER_REGION_SIZE];
} FontCharacterRegion;

LIST_DEFINE_H(FontCharacterRegionList, character_region_list, FontCharacterRegion*)
LIST_DEFINE_C(FontCharacterRegionList, character_region_list, FontCharacterRegion*)

struct FontImplTtf {
    FontCharacterRegionList regions;
    TextureList textures;
    TTF_Font* font;
    bool owns_font;
    float line_spacing;
    float spacing;
    float baseline;
    int letter_width;
    int letter_height;
};

static inline bool character_region_contains(FontCharacterRegion* region, Char32 letter) {
    return region->start <= letter && region->end >= letter;
}

FontImplTtf* font_ttf_create(TTF_Font* font, bool pass_ownership) {
    FontImplTtf* impl = soren_malloc(sizeof(*impl));
    impl->font = font;

    impl->letter_width = TTF_FontHeight(font);
    impl->letter_height = TTF_FontHeight(font);
    impl->line_spacing = (float)impl->letter_height;
    impl->owns_font = pass_ownership;
    impl->spacing = 0;
    impl->baseline = (float)(impl->letter_height + TTF_FontDescent(font));
    texture_list_init(&impl->textures);
    character_region_list_init(&impl->regions);

    return impl;
}

void font_ttf_free(FontImplTtf* font) {
    for (int i = 0; i < texture_list_count(&font->textures); i++) {
        SDL_DestroyTexture(texture_list_get(&font->textures, i));
    }

    for (int i = 0; i < character_region_list_count(&font->regions); i++) {
        FontCharacterRegion* region = character_region_list_get(&font->regions, i);
        soren_free(region);
    }

    texture_list_free_resources(&font->textures);
    character_region_list_free_resources(&font->regions);

    if (font->owns_font) {
        TTF_CloseFont(font->font);
    }
}

float font_ttf_line_height(FontImplTtf* font) {
    return font->line_spacing;
}

float font_ttf_letter_spacing(FontImplTtf* font) {
    return font->spacing;
}

void font_ttf_set_letter_spacing(FontImplTtf* font, float spacing) {
    font->spacing = spacing;
}

FontCharacterRegion* font_ttf_get_region(SDL_Renderer* renderer, FontImplTtf* font, Char32 letter) {
    for (int i = 0; i < character_region_list_count(&font->regions); i++) {
        FontCharacterRegion* region = character_region_list_get(&font->regions, i);
        if (character_region_contains(region, letter)) {
            return region;
        }
    }

    int sdl_result = 0;

    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA32, 
        SDL_TEXTUREACCESS_TARGET,
        font->letter_width * CHARACTER_REGION_WIDTH,
        font->letter_height * CHARACTER_REGION_WIDTH);

    SOREN_SDL_ASSERT(texture);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    sdl_result = SDL_SetRenderTarget(renderer, texture);
    SOREN_SDL_ASSERT(sdl_result == 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_Color white = (SDL_Color){ 255, 255, 255, 255 };

    Char32 start = letter - (letter % CHARACTER_REGION_SIZE);
    Char32 end = start + CHARACTER_REGION_SIZE;
    FontCharacterRegion* result = soren_malloc(sizeof(*result));
    result->start = start;
    result->end = end;
    result->texture_index = texture_list_count(&font->textures);

    Char32 index = start;
    for (int y = 0; y < CHARACTER_REGION_WIDTH; y++) {
        for (int x = 0; x < CHARACTER_REGION_WIDTH; x++) {
            GlyphInfo* glyph = &result->glyphs[y * CHARACTER_REGION_WIDTH + x];
            if (!TTF_GlyphIsProvided32(font->font, index)) {
                glyph->width = -1;
                index++;
                continue;
            }

            int minx;
            int miny;
            int maxx;
            int maxy;
            int advance;
            TTF_GlyphMetrics32(
                font->font,
                index,
                &minx,
                &maxx,
                &miny,
                &maxy,
                &advance);

            float glyph_width = (float)(maxx - minx);
            float glyph_height = (float)(font->baseline - miny);

            SDL_Surface* surface = TTF_RenderGlyph32_Solid(font->font, index, white);
            SOREN_SDL_ASSERT(surface);
            SDL_Texture* glyph_texture = SDL_CreateTextureFromSurface(renderer, surface);
            SOREN_SDL_ASSERT(glyph_texture);

            RectF dest = (RectF){
                (float)(x * font->letter_width),
                (float)(y * font->letter_height),
                glyph_width,
                glyph_height
            };

            glyph->texture_bounds = dest;
            // glyph->left_bearing = (float)minx;
            glyph->left_bearing = 0;
            glyph->right_bearing = (float)(advance - maxx);
            glyph->width = (float)(maxx - minx);

            sdl_result = SDL_RenderTexture(
                renderer,
                glyph_texture,
                &(RectF){ (float)minx, 0, glyph_width, glyph_height },
                &dest);

            SOREN_SDL_ASSERT(sdl_result == 0);

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(glyph_texture);

            index++;
        }
    }

    sdl_result = SDL_SetRenderTarget(renderer, NULL);
    SOREN_SDL_ASSERT(sdl_result == 0);

    texture_list_add(&font->textures, texture);
    character_region_list_add(&font->regions, result);

    return result;
}

Vector font_ttf_measure(FontImplTtf* font, SDL_Renderer* renderer, const char* str, int count) {
    soren_assert(font);
    soren_assert(renderer);
    if (!str || *str == 0) {
        return VECTOR_ZERO;
    }

    if (count <= 0) {
        count = INT_MAX;
    }

    Vector size = VECTOR_ZERO;
    FontCharacterRegion* last_region = NULL;
    bool first_glyph_of_line = true;
    Vector offset = VECTOR_ZERO;

    int index = 0;
    int codepoint_size;
    Char32 prev_char;
    Char32 character;
    while (index < count && (character = sso_string_u8_next(str + index, &codepoint_size))) {
        index += codepoint_size;

        if (character == '\r') {
            prev_char = character;
            continue;
        }

        if (character == '\n') {
            offset.x = 0;
            offset.y += font->line_spacing;
            first_glyph_of_line = true;
            prev_char = character;
            continue;
        }

        if (last_region == NULL || !character_region_contains(last_region, character)) {
            last_region = font_ttf_get_region(renderer, font, character);
        }

        int source_index = character - last_region->start;
        GlyphInfo* glyph = &last_region->glyphs[source_index];

        if (first_glyph_of_line) {
            first_glyph_of_line = false;
            offset.x += max(glyph->left_bearing, 0);
        } else {
            int kerning = TTF_GetFontKerningSizeGlyphs32(font->font, prev_char, character);
            offset.x += font->spacing + kerning + glyph->left_bearing;
        }

        offset.x += glyph->width + glyph->right_bearing;
        if (offset.x > size.x) {
            size.x = offset.x;
        }
        prev_char = character;
    }

    size.y = offset.y + font->line_spacing;

    return size;
}

void font_ttf_draw(
    FontImplTtf* font,
    SDL_Renderer* renderer,
    const char* str,
    int count,
    Vector position,
    SDL_Color color)
{
    soren_assert(font);
    soren_assert(renderer);
    if (!str || *str == 0) {
        return;
    }

    if (count <= 0) {
        count = INT_MAX;
    }

    graphics_set_color(renderer, color);

    FontCharacterRegion* last_region = NULL;
    SDL_Texture* last_texture = NULL;
    bool first_glyph_of_line = true;
    Vector offset = VECTOR_ZERO;

    int index = 0;
    int codepoint_size;
    Char32 prev_char;
    Char32 character;
    while (index < count && (character = sso_string_u8_next(str + index, &codepoint_size))) {
        index += codepoint_size;

        if (character == '\r') {
            prev_char = character;
            continue;
        }

        if (character == '\n') {
            offset.x = 0;
            offset.y += font->line_spacing;
            first_glyph_of_line = true;
            prev_char = character;
            continue;
        }

        if (last_region == NULL || !character_region_contains(last_region, character)) {
            last_region = font_ttf_get_region(renderer, font, character);
            last_texture = texture_list_get(&font->textures, last_region->texture_index);
            graphics_set_color(renderer, color);
        }

        int source_index = character - last_region->start;
        GlyphInfo* glyph = &last_region->glyphs[source_index];

        if (first_glyph_of_line) {
            first_glyph_of_line = false;
            offset.x += max(glyph->left_bearing, 0);
        } else {
            int kerning = TTF_GetFontKerningSizeGlyphs32(font->font, prev_char, character);
            offset.x += font->spacing + kerning + glyph->left_bearing;
        }

        Vector character_position = vector_add(position, offset);

        RectF dest = (RectF) {
            (float)character_position.x,
            (float)character_position.y,
            (float)glyph->texture_bounds.w,
            (float)glyph->texture_bounds.h
        };

        SDL_RenderTexture(renderer, last_texture, &glyph->texture_bounds, &dest);

        offset.x += glyph->width + glyph->right_bearing;
        prev_char = character;
    }
}

void font_ttf_draw_ext(
    FontImplTtf* font,
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
    soren_assert(font);
    soren_assert(renderer);
    if (!str || *str == 0) {
        return;
    }

    if (count <= 0) {
        count = INT_MAX;
    }

    Vector flip_adjustment = VECTOR_ZERO;
    bool flipped_vert = (flip & SDL_FLIP_VERTICAL) == SDL_FLIP_VERTICAL;
    bool flipped_horz = (flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL;

    if (flipped_vert || flipped_horz || rtl) {
        Vector size = font_ttf_measure(font, renderer, str, count);

        if (flipped_horz ^ rtl) {
            origin.x *= -1;
            flip_adjustment.x = -size.x;
        }

        if (flipped_vert) {
            origin.y *= -1;
            flip_adjustment.y = font->line_spacing - size.y;
        }
    }

    // Old transform matrix:
    // Matrix transform = matrix_create_trso(position, rotation, scale, origin);

    // Construct the matrix by hand to do some extra math for flipped strings.
    // Based on MonoGame SpriteFont rendering method in SpriteBatch
    Matrix transform;
    if (rotation == 0) {
        transform.m11 = (flipped_horz ? -scale.x : scale.x);
        transform.m22 = (flipped_vert ? -scale.y : scale.y);
        transform.m31 = ((flip_adjustment.x - origin.x) * transform.m11) + position.x;
        transform.m32 = ((flip_adjustment.y - origin.y) * transform.m22) + position.y;
    } else {
        float cos = SDL_cosf(rotation);
        float sin = SDL_sinf(rotation);
        
        transform.m11 = (flipped_horz ? -scale.x : scale.x) * cos;
        transform.m12 = (flipped_horz ? -scale.x : scale.x) * sin;
        transform.m21 = (flipped_vert ? -scale.y : scale.y) * -sin;
        transform.m22 = (flipped_vert ? -scale.y : scale.y) * cos;
        transform.m31 = (((flip_adjustment.x - origin.x) * transform.m11) + (flip_adjustment.y - origin.y) * transform.m21) + position.x;
        transform.m32 = (((flip_adjustment.x - origin.x) * transform.m12) + (flip_adjustment.y - origin.y) * transform.m22) + position.y;
    }

    // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    graphics_set_color(renderer, color);

    FontCharacterRegion* last_region = NULL;
    SDL_Texture* last_texture = NULL;
    bool first_glyph_of_line = true;
    Vector offset = VECTOR_ZERO;

    int index = 0;
    int codepoint_size;
    Char32 prev_char;
    Char32 character;
    while (index < count && (character = sso_string_u8_next(str + index, &codepoint_size))) {
        index += codepoint_size;

        if (character == '\r') {
            prev_char = character;
            continue;
        }

        if (character == '\n') {
            offset.x = 0;
            offset.y += font->line_spacing;
            first_glyph_of_line = true;
            prev_char = character;
            continue;
        }

        if (last_region == NULL || !character_region_contains(last_region, character)) {
            last_region = font_ttf_get_region(renderer, font, character);
            last_texture = texture_list_get(&font->textures, last_region->texture_index);
            graphics_set_color(renderer, color);
        }

        int source_index = character - last_region->start;
        GlyphInfo* glyph = &last_region->glyphs[source_index];
        float start_bearing = glyph->left_bearing;
        float end_bearing = glyph->right_bearing;
        if (rtl) {
            start_bearing = glyph->right_bearing;
            end_bearing = glyph->left_bearing;
        }

        if (first_glyph_of_line) {
            first_glyph_of_line = false;
            offset.x += max(start_bearing, 0);
        } else {
            int kerning = TTF_GetFontKerningSizeGlyphs32(font->font, prev_char, character);
            offset.x += font->spacing + kerning + start_bearing;
        }

        Vector character_position = offset;
        if (flipped_horz) {
            character_position.x += glyph->texture_bounds.w;
        }

        if (flipped_vert) {
            character_position.y += glyph->texture_bounds.h - font->line_spacing;
        }

        character_position = vector_transform(character_position, &transform);

        RectF dest = (RectF) {
            (float)character_position.x,
            (float)character_position.y,
            (float)glyph->texture_bounds.w * scale.x,
            (float)glyph->texture_bounds.h * scale.y
        };

        SDL_RenderTextureRotated(
            renderer,
            last_texture,
            &glyph->texture_bounds,
            &dest,
            radians_to_degrees(rotation),
            &(Vector) { glyph->texture_bounds.w / 2, 0 },
            flip
        );

        offset.x += glyph->width + end_bearing;
        prev_char = character;
    }
}