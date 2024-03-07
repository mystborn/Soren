#include <graphics/soren_sprite.h>
#include <soren_enum_parser.h>
#include "../../soren_init.h"
#include <graphics/soren_graphics.h>

EVENT_DEFINE_1_H(SpriteAnimatorCycleCompleteEvent, sprite_animation_cycle_complete_event, SpriteAnimator*)
EVENT_DEFINE_C(SpriteAnimatorCycleCompleteEvent, sprite_animation_cycle_complete_event)

LIST_DEFINE_C(SpriteFrameList, soren_sprite_frame_list, SpriteFrame)
LIST_DEFINE_C(SpriteList, sprite_list, Sprite*)
MAP_DEFINE_C(SpriteMap, sprite_map, const char*, Sprite*, gds_fnv32, strcmp)

/*
    Basic texture vertices:

    0 ---- 1
    |      |
    |      |
    3 ---- 2

    Nine Patch Vertices:

    00 - 01 ------------- 04 - 05
     | 1 |        2        | 3 |
    02 - 03 ------------- 06 - 07
     |   |                 |   |
     | 4 |        5        | 6 |
     |   |                 |   |
    08 - 09 ------------- 12 - 13
     | 7 |        8        | 9 |
    10 - 11 ------------- 14 - 15

    The basic vertices use a different vertex layout because that's the way
    polygons are supposed to be layed out for the sake of collisions, but it's
    way easier to reason about the nine patch vertices if they're layed out
    as they are in the diagram.
*/

static int texture_indices_table[] = { 0, 1, 2, 0, 3, 2 };

static int nine_patch_indices_table[] = {
    // Center
    3, 6, 12,
    3, 9, 12,

    // Top Left
    0, 1, 3,
    0, 2, 3,

    // Top Right
    4, 5, 7,
    4, 6, 7,

    // Bottom Left
    8, 9, 10,
    8, 10, 11,

    // Bottom Right
    12, 13, 15,
    12, 14, 15,

    // Top
    1, 4, 6,
    1, 3, 6,

    // Left
    2, 3, 9,
    2, 8, 9,

    // Bottom
    9, 12, 14,
    9, 11, 14,

    // Right
    6, 7, 13,
    6, 12, 13
};

static void texture_draw_rect_impl(
    SDL_Texture* texture, 
    float texel_width,
    float texel_height, 
    SDL_Renderer* renderer, 
    RectF source, 
    RectF dest, 
    SDL_FColor color, 
    float rotation, 
    Vector origin, 
    Vector scale, 
    SDL_FlipMode flip)
{
    Matrix transform = matrix_create_trso(rectf_location(dest), rotation, scale, origin);
    Vector points[4] = {
        { 0, 0 },
        { dest.w, 0 },
        { dest.w, dest.h },
        { 0, source.h }
    };

    vector_transform_batch(points, 4, points, &transform);

    if (graphics_using_camera(renderer, &transform)) {
        vector_transform_batch(points, 4, points, &transform);
    }

    Vector tex_coord_tl = vector_create(source.x * texel_width, source.y * texel_height);
    Vector tex_coord_br = vector_create((source.x + source.w) * texel_width, (source.y + source.h) * texel_height);

    if ((flip & SDL_FLIP_VERTICAL) != 0) {
        float temp = tex_coord_br.y;
        tex_coord_br.y = tex_coord_tl.y;
        tex_coord_tl.y = temp;
    }

    if ((flip & SDL_FLIP_HORIZONTAL) != 0) {
        float temp = tex_coord_br.x;
        tex_coord_br.x = tex_coord_tl.x;
        tex_coord_tl.x = temp;
    }

    Vector texture_coords[4] = {
        { tex_coord_tl.x, tex_coord_tl.y },
        { tex_coord_br.x, tex_coord_tl.y },
        { tex_coord_br.x, tex_coord_br.y },
        { tex_coord_tl.x, tex_coord_br.y }
    };

    // Really hacky behaviour!!!
    // Reinterpret the array of points and texture_coords as arrays of floats
    // such that array[0] == vector[0].x && array[1] == vector[0].y
    //
    // In addition, set the color_stride to 0 so that it always just uses the
    // same color value.

    SDL_RenderGeometryRawFloat(
        renderer,
        texture,
        (float*)points,
        sizeof(Vector),
        &color,
        0,
        (float*)texture_coords,
        sizeof(Vector),
        4,
        texture_indices_table,
        6,
        sizeof(int));
}

SOREN_EXPORT void texture_draw_rect(SDL_Texture* texture, SDL_Renderer* renderer, RectF source, RectF dest) {
    texture_draw_rect_ext(texture, renderer, source, dest, soren_colors.white, 0, VECTOR_ZERO, VECTOR_ONE, SDL_FLIP_NONE);
}

SOREN_EXPORT void texture_draw_rect_ext(SDL_Texture* texture, SDL_Renderer* renderer, RectF source, RectF dest, SDL_FColor color, float rotation, Vector origin, Vector scale, SDL_FlipMode flip) {
    int width;
    int height;
    int sdl_result = SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SOREN_SDL_ASSERT(sdl_result == 0);

    float texel_width = 1 / (float)width;
    float texel_height = 1 / (float)height;

    texture_draw_rect_impl(
        texture,
        texel_width,
        texel_height,
        renderer,
        source,
        dest,
        color,
        rotation,
        origin,
        scale,
        flip);
}

SOREN_EXPORT void nine_patch_texture_init(NinePatchTexture* nine_patch, SDL_Texture* texture, RectF source, Padding borders) {
    if (padding_width(&borders) > source.w || padding_height(&borders) > source.h) {
        throw(IllegalArgumentException, "Nine patch borders can't be larger than the source rect");
    }

    NinePatchDescription* description = soren_malloc(sizeof(*description));
    description->borders = borders;

    int width;
    int height;
    int sdl_result = SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SOREN_SDL_ASSERT(sdl_result == 0);

    float texel_width = 1 / (float)width;
    float texel_height = 1 / (float)height;

    float left_middle = source.w - (float)borders.right;
    float top_middle = source.h - (float)borders.bottom;

    description->uv_coords[0] = (Vector){ (source.x) * texel_width, (source.y) * texel_width };
    description->uv_coords[1] = (Vector){ (source.x + (float)borders.left) * texel_width, (source.y) * texel_width };
    description->uv_coords[2] = (Vector){ (source.x) * texel_width, (source.y + (float)borders.top) * texel_width };
    description->uv_coords[3] = (Vector){ (source.x + (float)borders.left) * texel_width, (source.y + (float)borders.top) * texel_width };
    description->uv_coords[4] = (Vector){ (source.x + left_middle) * texel_width, (source.y) * texel_width };
    description->uv_coords[5] = (Vector){ (source.x + source.w) * texel_width, (source.y) * texel_width };
    description->uv_coords[6] = (Vector){ (source.x + left_middle) * texel_width, (source.y + (float)borders.top) * texel_width };
    description->uv_coords[7] = (Vector){ (source.x + source.w) * texel_width, (source.y + (float)borders.top) * texel_width };
    description->uv_coords[8] = (Vector){ (source.x) * texel_width, (source.y + top_middle) * texel_width };
    description->uv_coords[9] = (Vector){ (source.x + (float)borders.left) * texel_width, (source.y + top_middle) * texel_width };
    description->uv_coords[10] = (Vector){ (source.x) * texel_width, (source.y + source.h) * texel_width };
    description->uv_coords[11] = (Vector){ (source.x + (float)borders.left) * texel_width, (source.y + source.h) * texel_width };
    description->uv_coords[12] = (Vector){ (source.x + left_middle) * texel_width, (source.y + top_middle) * texel_width };
    description->uv_coords[13] = (Vector){ (source.x + source.w) * texel_width, (source.y + top_middle) * texel_width };
    description->uv_coords[14] = (Vector){ (source.x + left_middle) * texel_width, (source.y + source.h) * texel_width };
    description->uv_coords[15] = (Vector){ (source.x + source.w) * texel_width, (source.y + source.h) * texel_width };

    nine_patch->description = description;
    nine_patch->source = source;
    nine_patch->texture = texture;
}

SOREN_EXPORT NinePatchTexture* nine_patch_texture_create(SDL_Texture* texture, RectF source, Padding borders) {
    NinePatchTexture* nine_patch = soren_malloc(sizeof(*nine_patch));
    nine_patch_texture_init(nine_patch, texture, source, borders);
    return nine_patch;
}

SOREN_EXPORT void nine_patch_texture_free_resources(NinePatchTexture* nine_patch) {
    soren_free(nine_patch->description);
}

SOREN_EXPORT void nine_patch_texture_free(NinePatchTexture* nine_patch) {
    nine_patch_texture_free_resources(nine_patch);
    soren_free(nine_patch);
}

SOREN_EXPORT void nine_patch_draw(NinePatchTexture* nine_patch, SDL_Renderer* renderer, RectF dest) {
    nine_patch_draw_ext(nine_patch, renderer, dest, soren_colors.white, 0, VECTOR_ZERO, VECTOR_ONE, SDL_FLIP_NONE);
}

SOREN_EXPORT void nine_patch_draw_ext(NinePatchTexture* nine_patch, SDL_Renderer* renderer, RectF dest, SDL_FColor color, float rotation, Vector origin, Vector scale, SDL_FlipMode flip) {
    if (flip != SDL_FLIP_NONE) {
        throw(NotImplementedException, "Flipping a nine patch texture is currently not implemented");
    }

    float center_width = dest.w - padding_width(&nine_patch->description->borders);
    float center_height = dest.h - padding_height(&nine_patch->description->borders);

    // This indicates that the destination is smaller than the texture source.
    // Many different things could be done in such a case. Currently this just
    // draws the entire source rect scaled down to the destination.
    if (center_width < 0 || center_height < 0) {
        texture_draw_rect_ext(nine_patch->texture, renderer, nine_patch->source, dest, color, rotation, origin, scale, flip);
        return;
    }

    float center_left = (float)nine_patch->description->borders.left;
    float center_right = center_left + center_width;
    float center_top = (float)nine_patch->description->borders.top;
    float center_bottom = center_top + center_height;
    float right = center_right + (float)nine_patch->description->borders.right;
    float bottom = center_bottom + (float)nine_patch->description->borders.bottom;

    Vector points[16] = {
        { 0, 0 },
        { center_left, 0 },
        { 0, center_top },
        { center_left, center_top },

        { center_right, 0 },
        { right, 0 },
        { center_right, center_top },
        { right, center_top },

        { 0, center_bottom },
        { center_left, center_bottom },
        { 0, bottom },
        { center_left, center_bottom },

        { center_right, center_bottom },
        { right, center_bottom },
        { center_right, bottom },
        { right, bottom }
    };

    Matrix transform = matrix_create_trso(rectf_location(dest), rotation, scale, origin);

    vector_transform_batch(points, 16, points, &transform);

    if (graphics_using_camera(renderer, &transform)) {
        vector_transform_batch(points, 4, points, &transform);
    }

    int index_start = 0;
    int index_count = 54;

    if (center_width == 0 || center_height == 0) {
        index_start = 6;
        index_count = 48;
    } else if (nine_patch->description->stamp_center) {
        index_start = 6;
        index_count = 48;
        throw(NotImplementedException, "Nine patch stamping not implemented yet");
    }

    if (nine_patch->description->stamp_borders) {
        index_count -= 24;
        throw(NotImplementedException, "Nine patch stamping not implemented yet");
    }

    // Really hacky behaviour!!!
    // Reinterpret the array of points and texture_coords as arrays of floats
    // such that array[0] == vector[0].x && array[1] == vector[0].y
    //
    // In addition, set the color_stride to 0 so that it always just uses the
    // same color value.

    SDL_RenderGeometryRawFloat(
        renderer,
        nine_patch->texture,
        (float*)points,
        sizeof(Vector),
        &color,
        0,
        (float*)nine_patch->description->uv_coords,
        sizeof(Vector),
        16,
        nine_patch_indices_table + index_start,
        index_count,
        sizeof(int));
}

SOREN_EXPORT bool sprite_animator_cycle_completed_subscribe(SpriteAnimator* animator, void* context, void (*cycle_completed)(void* context, SpriteAnimator* animator)) {
    if (!animator->cycle_completed) {
        animator->cycle_completed = sprite_animation_cycle_complete_event_create();
    }
    return sprite_animation_cycle_complete_event_subscribe(animator->cycle_completed, context, cycle_completed);
}

SOREN_EXPORT bool sprite_animator_cycle_completed_unsubscribe(SpriteAnimator* animator, void* context, void (*cycle_completed)(void* context, SpriteAnimator* animator)) {
    if (!animator->cycle_completed) {
        animator->cycle_completed = sprite_animation_cycle_complete_event_create();
    }
    return sprite_animation_cycle_complete_event_unsubscribe(animator->cycle_completed, context, cycle_completed);
}

static void sprite_animator_next_frame(SpriteAnimator* animator) {
    animator->index += animator->direction;
    if (animator->index >= soren_sprite_frame_list_count(&animator->sprite->frames) || animator->index < 0) {
        switch (animator->sprite->update_mode) {
            case SPRITE_UPDATE_PING_PONG:
                animator->direction *= -1;
                animator->index += animator->direction * 2;
                break;
            case SPRITE_UPDATE_CYCLE:
                animator->index = 0;
                break;
            case SPRITE_UPDATE_ONCE:
                animator->index = SPRITE_ANIMATION_COMPLETE;
                break;
            case SPRITE_UPDATE_ONCE_STAY:
                animator->direction = 0;
                break;
        }

        if (animator->cycle_completed) {
            sprite_animation_cycle_complete_event_trigger(animator->cycle_completed, animator);
        }
    }
}

SOREN_EXPORT void sprite_animator_update(SpriteAnimator* animator, float delta) {
    if (sprite_animator_paused(animator)
        || sprite_animator_complete(animator)
        || !animator->sprite
        || animator->sprite->update_mode == SPRITE_UPDATE_NONE
        || (animator->sprite->update_mode == SPRITE_UPDATE_ONCE_STAY && animator->index 
            == soren_sprite_frame_list_count(&animator->sprite->frames) - 1))
    {
        return;
    }

    animator->ticks += delta;
    while (animator->ticks >= animator->max_ticks && !sprite_animator_complete(animator)) {
        animator->ticks -= animator->max_ticks;

        sprite_animator_next_frame(animator);
    }
}

SOREN_EXPORT sprite_draw_rect(Sprite* sprite, SDL_Renderer* renderer, int frame, RectF dest) {
    sprite_draw_rect_ext(sprite, renderer, frame, dest, soren_colors.white, 0, VECTOR_ONE, SDL_FLIP_NONE);
}

SOREN_EXPORT sprite_draw_rect_ext(Sprite* sprite, SDL_Renderer* renderer, int frame, RectF dest, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip) {
    SpriteFrame sprite_frame = soren_sprite_frame_list_get(&sprite->frames, frame);
    texture_draw_rect_impl(
        sprite->texture,
        sprite->texel_width,
        sprite->texel_height,
        renderer,
        sprite_frame.bounds,
        dest,
        color,
        rotation,
        sprite->origin,
        scale,
        flip);
}

SOREN_EXPORT sprite_draw_pos(Sprite* sprite, SDL_Renderer* renderer, int frame, Vector position) {
    sprite_draw_pos_ext(sprite, renderer, frame, position, soren_colors.white, 0, VECTOR_ONE, SDL_FLIP_NONE);
}

SOREN_EXPORT void sprite_draw_pos_ext(Sprite* sprite, SDL_Renderer* renderer, int frame, Vector position, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip) {
    SpriteFrame sprite_frame = soren_sprite_frame_list_get(&sprite->frames, frame);
    RectF dest = (RectF){
        position.x - sprite->origin.x,
        position.y - sprite->origin.y,
        sprite_frame.bounds.w,
        sprite_frame.bounds.h
    };

    texture_draw_rect_impl(
        sprite->texture,
        sprite->texel_width,
        sprite->texel_height,
        renderer,
        sprite_frame.bounds,
        dest,
        color,
        rotation,
        sprite->origin,
        scale,
        flip);

    // Matrix transform = matrix_create_trso(vector_subtract(position, sprite->origin), rotation, scale, sprite->origin);
    // Vector points[4] = {
    //     { 0, 0 },
    //     { source.w, 0 },
    //     { source.w, source.h },
    //     { 0, source.h }
    // };

    // vector_transform_batch(points, 4, points, &transform);

    // if (graphics_using_camera(renderer, &transform)) {
    //     vector_transform_batch(points, 4, points, &transform);
    // }

    // float texel_width = sprite->texel_width;
    // float texel_height = sprite->texel_height;

    // Vector tex_coord_tl = vector_create(source.x * texel_width, source.y * texel_height);
    // Vector tex_coord_br = vector_create((source.x + source.w) * texel_width, (source.y + source.h) * texel_height);

    // if ((flip & SDL_FLIP_VERTICAL) != 0) {
    //     float temp = tex_coord_br.y;
    //     tex_coord_br.y = tex_coord_tl.y;
    //     tex_coord_tl.y = temp;
    // }

    // if ((flip & SDL_FLIP_HORIZONTAL) != 0) {
    //     float temp = tex_coord_br.x;
    //     tex_coord_br.x = tex_coord_tl.x;
    //     tex_coord_tl.x = temp;
    // }

    // Vector texture_coords[4] = {
    //     { tex_coord_tl.x, tex_coord_tl.y },
    //     { tex_coord_br.x, tex_coord_tl.y },
    //     { tex_coord_br.x, tex_coord_br.y },
    //     { tex_coord_tl.x, tex_coord_br.y }
    // };

    // SDL_Vertex vertices[4];
    // for (int i = 0; i < 4; i++) {
    //     vertices[i].color = color;
    //     vertices[i].position = points[i];
    //     vertices[i].tex_coord = texture_coords[i];
    // }

    // int indices[6] = { 0, 1, 2, 0, 2, 3 };
    
    // SDL_RenderGeometry(renderer, sprite->texture, vertices, 4, indices, 6);
}

SOREN_EXPORT void sprite_animator_draw_rect_ext(SpriteAnimator* animator, SDL_Renderer* renderer, RectF dest, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip) {
    if (!animator->sprite || sprite_animator_complete(animator)) {
        return;
    }

    sprite_draw_rect_ext(animator->sprite, renderer, animator->index, dest, color, rotation, scale, flip);
}

SOREN_EXPORT void sprite_animator_draw_rect(SpriteAnimator* animator, SDL_Renderer* renderer, RectF dest) {
    if (!animator->sprite || sprite_animator_complete(animator)) {
        return;
    }

    sprite_draw_rect_ext(animator->sprite, renderer, animator->index, dest, soren_colors.white, 0, VECTOR_ONE, SDL_FLIP_NONE);
}

SOREN_EXPORT void sprite_animator_draw_pos_ext(SpriteAnimator* animator, SDL_Renderer* renderer, Vector position, SDL_FColor color, float rotation, Vector scale, SDL_FlipMode flip) {
    if (!animator->sprite || sprite_animator_complete(animator)) {
        return;
    }

    sprite_draw_pos_ext(animator->sprite, renderer, animator->index, position, color, rotation, scale, flip);
}

SOREN_EXPORT void sprite_animator_draw_pos(SpriteAnimator* animator, SDL_Renderer* renderer, Vector position) {
    if (!animator->sprite || sprite_animator_complete(animator)) {
        return;
    }

    sprite_draw_pos_ext(animator->sprite, renderer, animator->index, position, soren_colors.white, 0, VECTOR_ONE, SDL_FLIP_NONE);
}