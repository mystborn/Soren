#include <resources/soren_resources.h>
#include <generic_map.h>
#include "../soren_init.h"
#include <SDL3_image/SDL_image.h>
#include <generic_iterators/map_iterator.h>
#include <sso_string.h>

typedef struct SorenResource {
    void* resource;
    String type_name;
    int ref_count;
} SorenResource;

MAP_DEFINE_H(ResourceMap, resource_map, const char*, SorenResource*)
MAP_DEFINE_C(ResourceMap, resource_map, const char*, SorenResource*, gds_fnv32, strcmp)

static ResourceMap soren_resources;

void soren_resource_map_init(void) {
    resource_map_init(&soren_resources);
}

static inline bool resource_unload_impl(void* value, void (free_fn)(void*)) {
    const char* key;
    SorenResource* resource;

    map_iter_start(&soren_resources, key, resource) {
        if (resource->resource == value) {
            log_trace(soren_logger, "Unloading %s: %s", string_data(&resource->type_name), key);
            if (--resource->ref_count == 0) {
                log_trace(soren_logger, "Destroying %s: %s", string_data(&resource->type_name), key);
                resource_map_remove(&soren_resources, key);
                free_fn(value);
                string_free_resources(&resource->type_name);
                soren_free(resource);
            }
            return true;
        }
    }
    map_iter_end

    return false;
}

SOREN_EXPORT SDL_Texture* resource_load_texture(SDL_Renderer* renderer, const char* fname) {
    SorenResource* resource = NULL;

    log_trace(soren_logger, "Loading texture: %s", fname);

    if (!resource_map_try_get(&soren_resources, fname, &resource)) {
        log_trace(soren_logger, "Initializing texture: %s", fname);
        fname = strdup(fname);
        resource = soren_malloc(sizeof(*resource));
        SDL_Texture* texture = IMG_LoadTexture(renderer, fname);
        SOREN_SDL_ASSERT(texture);
        resource->resource = texture;
        string_init(&resource->type_name, "SDL_Texture*");

        resource_map_add(&soren_resources, fname, resource);
    }

    resource->ref_count++;
    return (SDL_Texture*)resource->resource;
}

SOREN_EXPORT void resource_unload_texture(SDL_Texture* texture) {
    resource_unload_impl(texture, SDL_DestroyTexture);
}

SOREN_EXPORT SpriteAtlas* resource_load_sprite_atlas(SDL_Renderer* renderer, const char* fname) {
    SorenResource* resource;

    if (!resource_map_try_get(&soren_resources, fname, &resource)) {
        resource = soren_malloc(sizeof(*resource));
        SpriteAtlas* atlas = sprite_atlas_create_from_json_file(renderer, fname);
        resource->resource = atlas;
        string_init(&resource->type_name, "SpriteAtlas*");

        resource_map_add(&soren_resources, fname, resource);
    }

    resource->ref_count++;
    return (SpriteAtlas*)resource->resource;
}

SOREN_EXPORT void resource_unload_sprite_atlas(SpriteAtlas* atlas) {
    resource_unload_impl(atlas, sprite_atlas_free);
}