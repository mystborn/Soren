#ifndef SOREN_GRAPHICS_SOREN_SPRITE_H
#define SOREN_GRAPHICS_SOREN_SPRITE_H

#include "../soren_std.h"
#include "../soren_math.h"
#include "../soren_generics.h"
#include "../soren_enum_parser.h"
#include "soren_padding.h"

#include <sso_string.h>

typedef enum SpriteUpdateMode {
    SPRITE_UPDATE_NONE,
    SPRITE_UPDATE_ONCE,
    SPRITE_UPDATE_ONCE_STAY,
    SPRITE_UPDATE_PING_PONG,
    SPRITE_UPDATE_CYCLE
} SpriteUpdateMode;

SOREN_EXPORT SpriteUpdateMode sprite_update_mode_parse(String* str);
SOREN_EXPORT bool sprite_update_mode_try_parse(String* str, SpriteUpdateMode* out_mode);
SOREN_EXPORT String* sprite_update_mode_to_string(SpriteUpdateMode mode, String* str, bool append_prefix);

typedef struct NinePatchDescription {
    Vector uv_coords[16];
    Padding borders;

    /// Determines if the border is stretched or copy and pasted when the nine patch is drawn.
    bool stamp_borders;

    /// Determines if the center is stretched or copy and pasted when the nine patch is drawn.
    bool stamp_center;
} NinePatchDescription;

typedef struct NinePatchTexture {
    SDL_Texture* texture;
    RectF source;
    NinePatchDescription* description;
} NinePatchTexture;

SOREN_EXPORT void texture_draw_rect(SDL_Texture* texture, SDL_Renderer* renderer, RectF source, RectF dest);
SOREN_EXPORT void texture_draw_rect_ext(SDL_Texture* texture, SDL_Renderer* renderer, RectF source, RectF dest, SDL_FColor color, float rotation, Vector origin, Vector scale, SDL_FlipMode flip);

SOREN_EXPORT void nine_patch_texture_init(NinePatchTexture* nine_patch, SDL_Texture* texture, RectF source, Padding borders);
SOREN_EXPORT NinePatchTexture* nine_patch_texture_create(SDL_Texture* texture, RectF source, Padding borders);
SOREN_EXPORT void nine_patch_texture_free_resources(NinePatchTexture* nine_patch);
SOREN_EXPORT void nine_patch_texture_free(NinePatchTexture* nine_patch);

SOREN_EXPORT void nine_patch_draw(NinePatchTexture* nine_patch, SDL_Renderer* renderer, RectF dest);
SOREN_EXPORT void nine_patch_draw_ext(NinePatchTexture* nine_patch, SDL_Renderer* renderer, RectF dest, SDL_FColor color, float rotation, Vector origin, Vector scale, SDL_FlipMode flip);

typedef struct SpriteFrame {
    RectF bounds;
    NinePatchDescription* description;
} SpriteFrame;

LIST_DEFINE_H(SpriteFrameList, soren_sprite_frame_list, SpriteFrame)

typedef struct Sprite {
    String name;
    SDL_Texture* texture;
    SpriteUpdateMode update_mode;
    float frames_per_second;
    Vector origin;
    SpriteFrameList frames;
    int starting_frame;
    float texel_width;
    float texel_height;
} Sprite;

SOREN_EXPORT sprite_draw_rect(Sprite* sprite, SDL_Renderer* renderer, int frame, RectF dest);
SOREN_EXPORT sprite_draw_rect_ext(Sprite* sprite, SDL_Renderer* renderer, int frame, RectF dest, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip);

SOREN_EXPORT sprite_draw_pos(Sprite* sprite, SDL_Renderer* renderer, int frame, Vector position);
SOREN_EXPORT void sprite_draw_pos_ext(Sprite* sprite, SDL_Renderer* renderer, int frame, Vector position, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip);

LIST_DEFINE_H(SpriteList, sprite_list, Sprite*)
MAP_DEFINE_H(SpriteMap, sprite_map, const char*, Sprite*)

typedef struct SpriteAtlas {
    String name;
    SpriteList* animation_list;
    SpriteMap* animations;
} SpriteAtlas;

SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_json_file(SDL_Renderer* renderer, const char* file);
SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_json_string(SDL_Renderer* renderer, const char* json);
SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_map(SpriteMap* sprites);
SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_list(SpriteList* sprites);
SOREN_EXPORT void sprite_atlas_init_from_json_file(SpriteAtlas* atlas, SDL_Renderer* renderer, const char* file);
SOREN_EXPORT void sprite_atlas_init_from_json_string(SpriteAtlas* atlas, SDL_Renderer* renderer, const char* json);
SOREN_EXPORT void sprite_atlas_init_from_map(SpriteAtlas* atlas, SpriteMap* sprites);
SOREN_EXPORT void sprite_atlas_init_from_list(SpriteAtlas* atlas, SpriteList* sprites);
SOREN_EXPORT void sprite_atlas_free_resources(SpriteAtlas* atlas);
SOREN_EXPORT void sprite_atlas_free(SpriteAtlas* atlas);

typedef struct SpriteAnimator SpriteAnimator;
struct SpriteAnimatorCycleCompleteEvent;

#define SPRITE_ANIMATION_COMPLETE -1

struct SpriteAnimator {
    Sprite* sprite;
    struct SpriteAnimatorCycleCompleteEvent* cycle_completed;
    int index;
    int direction;
    float ticks;
    float max_ticks;
    bool paused;
};

SOREN_EXPORT bool sprite_animator_cycle_completed_subscribe(SpriteAnimator* animator, void* context, void (*cycle_completed)(void* context, SpriteAnimator* animator));
SOREN_EXPORT bool sprite_animator_cycle_completed_unsubscribe(SpriteAnimator* animator, void* context, void (*cycle_completed)(void* context, SpriteAnimator* animator));
SOREN_EXPORT void sprite_animator_update(SpriteAnimator* animator, float delta);
SOREN_EXPORT void sprite_animator_draw_rect(SpriteAnimator* animator, SDL_Renderer* renderer, RectF dest);
SOREN_EXPORT void sprite_animator_draw_rect_ext(SpriteAnimator* animator, SDL_Renderer* renderer, RectF dest, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip);
SOREN_EXPORT void sprite_animator_draw_pos(SpriteAnimator* animator, SDL_Renderer* renderer, Vector position);
SOREN_EXPORT void sprite_animator_draw_pos_ext(SpriteAnimator* animator, SDL_Renderer* renderer, Vector position, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip);

static inline Sprite* sprite_animator_sprite(SpriteAnimator* animator) {
    return animator->sprite;
}

static inline void sprite_animator_set_sprite(SpriteAnimator* animator, Sprite* sprite) {
    if (animator->sprite == sprite) {
        return;
    }

    animator->sprite = sprite;

    if (!sprite) {
        animator->index = SPRITE_ANIMATION_COMPLETE;
    } else {
        animator->paused = false;
        animator->ticks = 0;
        animator->direction = 1;
        animator->max_ticks = sprite->frames_per_second == 0 ? 0 : (1.f / sprite->frames_per_second);
        animator->index = sprite->starting_frame;
    }
}

static inline SpriteFrame sprite_animator_current_frame(SpriteAnimator* animator) {
    if (!animator->sprite 
        || animator->index < 0
        || animator->index >= soren_sprite_frame_list_count(&animator->sprite->frames))
    {
        return (SpriteFrame){0};
    }

    return soren_sprite_frame_list_get(&animator->sprite->frames, animator->index);
}

static inline int sprite_animator_frame_index(SpriteAnimator* animator) {
    return animator->index;
}

static inline void sprite_animator_set_frame_index(SpriteAnimator* animator, int index) {
    if (animator->index == index) {
        return;
    }

    if (!animator->sprite) {
        animator->index = index;
        return;
    }

    if (index < -1 || index >= soren_sprite_frame_list_count(&animator->sprite->frames)) {
        throw(IllegalArgumentException, "Tried to set frame index outside of sprite frames");
    }

    animator->index = index;
}

static inline bool sprite_animator_paused(SpriteAnimator* animator) {
    return animator->paused;
}

static inline void sprite_animator_set_paused(SpriteAnimator* animator, bool paused) {
    animator->paused = paused;
}

static inline bool sprite_animator_complete(SpriteAnimator* animator) {
    return animator->index == SPRITE_ANIMATION_COMPLETE
        || (animator->sprite && animator->sprite->update_mode == SPRITE_UPDATE_ONCE_STAY && animator->direction == 0);
}

static inline void sprite_animator_restart(SpriteAnimator* animator) {
    animator->ticks = 0;
    animator->direction = 1;
    animator->index = animator->sprite ? animator->sprite->starting_frame : 0;
}

#endif