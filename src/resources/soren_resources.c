#include <resources/soren_resources.h>
#include <generic_map.h>
#include "../soren_init.h"
#include <SDL3_image/SDL_image.h>
#include <generic_iterators/map_iterator.h>
#include <sso_string.h>

typedef struct SorenResource {
    void* resource;
    String* file;
    String type_name;
    int ref_count;
    void (*free)(void*);
} SorenResource;

MAP_DEFINE_H(ResourceMap, resource_map, String*, SorenResource*)
MAP_DEFINE_C(ResourceMap, resource_map, String*, SorenResource*, string_hash, string_compare_string)

MAP_DEFINE_H(PointerToResourceMap, ptrm, void*, SorenResource*)
MAP_DEFINE_C(PointerToResourceMap, ptrm, void*, SorenResource*, gds_pointer_hash, gds_pointer_compare);

static ResourceMap soren_resources;
static PointerToResourceMap soren_value_to_resource;

void soren_resource_map_init(void) {
    resource_map_init(&soren_resources);
    ptrm_init(&soren_value_to_resource);
}

SOREN_EXPORT void* resource_load(const char* fname, SDL_Renderer* renderer, const char* type, void* ctx, ResourceInitFn init_fn, void (*free_fn)(void* value)) {
    SorenResource* resource = NULL;
    String file_name = string_create(fname);

    log_trace(soren_logger, "Loading %s: %s", type, fname);

    if (!resource_map_try_get(&soren_resources, &file_name, &resource)) {
        log_trace(soren_logger, "Initializing %s: %s", type, fname);
        resource = soren_malloc(sizeof(*resource));
        resource->resource = init_fn(fname, renderer, ctx);
        string_init(&resource->type_name, type);
        resource->file = string_create_ref(fname);
        resource->free = free_fn;

        resource_map_add(&soren_resources, resource->file, resource);
        ptrm_add(&soren_value_to_resource, resource, resource);
    }

    resource->ref_count++;
    return resource->resource;
}

SOREN_EXPORT void resource_decrement(void* value) {
    SorenResource* resource;
    if (ptrm_try_get(&soren_value_to_resource, value, &resource)) {
        log_trace(soren_logger, "Unloading %s: %s", string_data(&resource->type_name), string_data(resource->file));
        if (--resource->ref_count == 0) {
            log_trace(soren_logger, "Destroying %s: %s", string_data(&resource->type_name), string_data(resource->file));

            // Remove from the resource maps
            resource_map_remove(&soren_resources, resource->file);
            ptrm_remove(&soren_value_to_resource, value);

            // Destroy the resource itself
            resource->free(value);

            // Free the resource container
            string_free_resources(&resource->type_name);
            string_free(resource->file);
            soren_free(resource);
        }
    }
}

static void* resource_load_texture_impl(const char* fname, SDL_Renderer* renderer, void* ctx) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, fname);
    SOREN_SDL_ASSERT(texture);
    return texture;
}

SOREN_EXPORT SDL_Texture* resource_load_texture(SDL_Renderer* renderer, const char* fname) {
    return (SDL_Texture*)resource_load(fname, renderer, "texture", NULL, resource_load_texture_impl, SDL_DestroyTexture);
}

static void* resource_load_sprite_atlas_impl(const char* fname, SDL_Renderer* renderer, void* ctx) {
    SpriteAtlas* atlas = sprite_atlas_create_from_json_file(renderer, fname);
    return atlas;
}

SOREN_EXPORT SpriteAtlas* resource_load_sprite_atlas(SDL_Renderer* renderer, const char* fname) {
    return (SpriteAtlas*)resource_load(fname, renderer, "sprite atlas", NULL, resource_load_sprite_atlas_impl, sprite_atlas_free);
}

SOREN_EXPORT bool resource_is_valid(void* resource) {
    return ptrm_try_get(&soren_value_to_resource, resource, NULL);
}

SOREN_EXPORT void resource_increment(void* ref) {
    SorenResource* resource;
    if (ptrm_try_get(&soren_value_to_resource, ref, &resource)) {
        resource->ref_count++;
    }
}