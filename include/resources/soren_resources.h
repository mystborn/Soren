#ifndef SOREN_RESOURCES_SOREN_RESOURCES_H
#define SOREN_RESOURCES_SOREN_RESOURCES_H

#include "../soren_std.h"
#include <SDL3/SDL.h>

#include "../graphics/soren_sprite.h"

SOREN_EXPORT SDL_Texture* resource_load_texture(SDL_Renderer* renderer, const char* fname);
SOREN_EXPORT void resource_unload_texture(SDL_Texture* texture);

SOREN_EXPORT SpriteAtlas* resource_load_sprite_atlas(SDL_Renderer* renderer, const char* fname);
SOREN_EXPORT void resource_unload_sprite_atlas(SpriteAtlas* atlas);

SOREN_EXPORT resource_is_valid(void* resource);

SOREN_EXPORT resource_increment(void* resource);
SOREN_EXPORT resource_decrement(void* resource);

#endif