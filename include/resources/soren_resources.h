#ifndef SOREN_RESOURCES_SOREN_RESOURCES_H
#define SOREN_RESOURCES_SOREN_RESOURCES_H

#include "../soren_std.h"
#include <SDL3/SDL.h>

#include "../graphics/soren_sprite.h"

typedef void* (*ResourceInitFn)(const char* fname, SDL_Renderer* renderer, void* ctx);

SOREN_EXPORT void* resource_load(const char* fname, SDL_Renderer* renderer, const char* type, void* ctx, ResourceInitFn on_load, void (*free_fn)(void* value));
SOREN_EXPORT void resource_decrement(void* resource);

SOREN_EXPORT SDL_Texture* resource_load_texture(SDL_Renderer* renderer, const char* fname);
SOREN_EXPORT SpriteAtlas* resource_load_sprite_atlas(SDL_Renderer* renderer, const char* fname);

SOREN_EXPORT bool resource_register(void* ref, char* key, char* type, void (*free_fn)(void* value));
SOREN_EXPORT bool resource_is_valid(void* resource);
SOREN_EXPORT void resource_increment(void* resource);

#endif