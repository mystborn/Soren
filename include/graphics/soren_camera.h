#ifndef SOREN_GRAPHICS_SOREN_CAMERA_H
#define SOREN_GRAPHICS_SOREN_CAMERA_H

#include "../soren_std.h"
#include "../soren_math.h"

typedef struct Camera {
    SDL_Renderer* renderer;
    SDL_Texture* render_target;
    RectF bounds;
    float rotation;
    Matrix matrix;
    bool matrix_dirty;
} Camera;

static inline void camera_init(Camera* camera, SDL_Renderer* renderer, int width, int height) {
    soren_assert(camera);
    soren_assert(renderer);
    soren_assert(width > 0 && height > 0);

    camera->renderer = renderer;
    camera->render_target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(camera->render_target, SDL_BLENDMODE_BLEND);
    camera->bounds = (RectF){ 0, 0, (float)width, (float)height };
    camera->rotation = 0;
    camera->matrix = MATRIX_IDENTITY;
    camera->matrix_dirty = true;
}

static inline Camera* camera_create(SDL_Renderer* renderer, int width, int height) {
    Camera* camera = soren_malloc(sizeof(*camera));
    camera_init(camera, renderer, width, height);
    return camera;
}

static inline Matrix camera_view_matrix(Camera* camera) {
    if (camera->matrix_dirty) {
        camera->matrix = matrix_create_tro(
            vector_negate(rectf_location(camera->bounds)),
            camera->rotation,
            rectf_center(camera->bounds));

        camera->matrix_dirty = false;
    }

    return camera->matrix;
}

static inline float camera_x(Camera* camera) {
    return camera->bounds.x;
}

static inline void camera_set_x(Camera* camera, float x) {
    camera->bounds.x = x;
    camera->matrix_dirty = true;
}

static inline float camera_y(Camera* camera) {
    return camera->bounds.y;
}

static inline void camera_set_y(Camera* camera, float y) {
    camera->bounds.y = y;
    camera->matrix_dirty = true;
}

static inline Vector camera_position(Camera* camera) {
    return rectf_location(camera->bounds);
}

static inline void camera_set_position(Camera* camera, Vector position) {
    camera->bounds.x = position.x;
    camera->bounds.y = position.y;
    camera->matrix_dirty = true;
}

static inline void camera_move(Camera* camera, Vector delta) {
    camera->bounds.x += delta.x;
    camera->bounds.y += delta.y;
    camera->matrix_dirty = true;
}

static inline void camera_look_at(Camera* camera, Vector point) {
    camera->bounds.x = point.x - camera->bounds.w / 2;
    camera->bounds.y = point.y - camera->bounds.h / 2;
    camera->matrix_dirty = true;
}

static inline float camera_rotation(Camera* camera) {
    return camera->rotation;
}

static inline void camera_set_rotation(Camera* camera, float rotation) {
    camera->rotation = rotation;
    camera->matrix_dirty = true;
}

static inline void camera_rotate(Camera* camera, float delta_radians) {
    camera->rotation += delta_radians;
    camera->matrix_dirty = true;
}

// TODO: Set camera size...

static inline float camera_width(Camera* camera) {
    return camera->bounds.w;
}

static inline float camera_height(Camera* camera) {
    return camera->bounds.h;
}

static inline Vector camera_size(Camera* camera) {
    return rectf_size(camera->bounds);
}

static inline RectF camera_get_bounds(Camera* camera) {
    return camera->bounds;
}

static inline SDL_Texture* camera_render_target(Camera* camera) {
    return camera->render_target;
}

#endif