#include <graphics/soren_sprite.h>
#include <graphics/soren_padding.h>
#include <resources/soren_resources.h>
#include <resources/soren_json.h>

#include <generic_iterators/map_iterator.h>
#include <generic_iterators/list_iterator.h>

#include <external/parson.h>

static RectF sprite_atlas_json_read_frame(JSON_Object* frame, float width, float height) {
    if (!json_object_has_value_of_type(frame, "x", JSONNumber)) {
        throw(JsonException, "Frame must have an X position");
    }

    if (!json_object_has_value_of_type(frame, "y", JSONNumber)) {
        throw(JsonException, "Frame must have a Y position");
    }

    float x = (float)json_object_get_number(frame, "x");
    float y = (float)json_object_get_number(frame, "y");
    
    if (json_object_has_value_of_type(frame, "width", JSONNumber)) {
        width = (float)json_object_get_number(frame, "width");
    }

    if (json_object_has_value_of_type(frame, "height", JSONNumber)) {
        height = (float)json_object_get_number(frame, "height");
    }

    if (x < 0) {
        throw(JsonException, "Invalid sprite frame. X cannot be < 0");
    }

    if (y < 0) {
        throw(JsonException, "Invalid sprite frame. Y cannot be < 0");
    }

    if (width <= 0) {
        throw(JsonException, "Invalid sprite frame. Width cannot be <= 0");
    }

    if (height <= 0) {
        throw(JsonException, "Invalid sprite frame. Height cannot be <= 0");
    }

    return (RectF){ x, y, width, height };
}

static void sprite_atlas_init_impl(SpriteAtlas* atlas, SDL_Renderer* renderer, JSON_Object* root) {
    atlas->animation_list = sprite_list_create();
    atlas->animations = sprite_map_create();

    String string_builder = string_create("");

    int texture_width;
    int texture_height;

    if (json_object_has_value_of_type(root, "name", JSONString)) {
        string_init(&atlas->name, json_object_get_string(root, "name"));
    } else {
        string_init(&atlas->name, "");
    }

    const char* default_texture = NULL;
    float default_width = -1;
    float default_height = -1;
    Padding default_padding = PADDING_EMPTY;
    float default_fps = 0;
    SpriteUpdateMode default_update_mode = SPRITE_UPDATE_NONE;
    int default_starting_frame = 0;
    Vector default_origin = VECTOR_ZERO;

    if (json_object_has_value_of_type(root, "texture", JSONString)) {
        default_texture = json_object_get_string(root, "texture");
    }

    if (json_object_has_value_of_type(root, "width", JSONNumber)) {
        default_width = (float)json_object_get_number(root, "width");
    }

    if (json_object_has_value_of_type(root, "height", JSONNumber)) {
        default_height = (float)json_object_get_number(root, "height");
    }

    if (json_object_has_value(root, "padding")) {
        default_padding = soren_json_parse_padding(json_object_get_value(root, "padding"));
    }

    if (json_object_has_value_of_type(root, "fps", JSONNumber)) {
        default_fps = (float)json_object_get_number(root, "fps");
    }

    if (json_object_has_value_of_type(root, "startingframe", JSONNumber)) {
        default_starting_frame = (int)json_object_get_number(root, "startingframe");
    }

    if (json_object_has_value_of_type(root, "starting_frame", JSONNumber)) {
        default_starting_frame = (int)json_object_get_number(root, "starting_frame");
    }

    if (json_object_has_value_of_type(root, "updatemode", JSONString)) {
        string_clear(&string_builder);
        string_append(&string_builder, json_object_get_string(root, "updatemode"));
        default_update_mode = sprite_update_mode_parse(&string_builder);
    }

    if (json_object_has_value_of_type(root, "update_mode", JSONString)) {
        string_clear(&string_builder);
        string_append(&string_builder, json_object_get_string(root, "update_mode"));
        default_update_mode = sprite_update_mode_parse(&string_builder);
    }

    if (json_object_has_value(root, "origin")) {
        default_origin = soren_json_parse_vector(json_object_get_value(root, "origin"));
    }

    if (json_object_has_value_of_type(root, "animations", JSONArray)) {
        JSON_Array* animation_array = json_object_get_array(root, "animations");
        size_t animation_count = json_array_get_count(animation_array);
        for (int i = 0; i < animation_count; i++) {
            JSON_Value* frame_element = json_array_get_value(animation_array, i);
            if (json_value_get_type(frame_element) != JSONObject) {
                throw(JsonException, "Expected a sprite object in the animation array");
            }

            JSON_Object* frame = json_object(frame_element);

            Sprite* sprite = soren_malloc(sizeof(*sprite));
            soren_sprite_frame_list_init(&sprite->frames);

            if (json_object_has_value_of_type(frame, "name", JSONString)) {
                string_init(&sprite->name, json_object_get_string(frame, "name"));
                if (string_u8_is_null_or_whitespace(&sprite->name)) {
                    throw(JsonException, "Invalid sprite name. Cannot be an empty string.");
                }
            } else {
                string_init(&sprite->name, "");
                string_format(&sprite->name, "%d", i);
            }

            sprite_list_add(atlas->animation_list, sprite);
            sprite_map_add(atlas->animations, string_data(&sprite->name), sprite);

            if (json_object_has_value_of_type(frame, "updatemode", JSONString)) {
                string_clear(&string_builder);
                string_append(&string_builder, json_object_get_string(frame, "updatemode"));
                sprite->update_mode = sprite_update_mode_parse(&string_builder);
            } else if (json_object_has_value_of_type(frame, "update_mode", JSONString)) {
                string_clear(&string_builder);
                string_append(&string_builder, json_object_get_string(frame, "update_mode"));
                sprite->update_mode = sprite_update_mode_parse(&string_builder);
            } else {
                sprite->update_mode = default_update_mode;
            }

            if (json_object_has_value(frame, "origin")) {
                sprite->origin = soren_json_parse_vector(json_object_get_value(frame, "origin"));
            } else {
                sprite->origin = default_origin;
            }

            if (json_object_has_value_of_type(frame, "fps", JSONNumber)) {
                sprite->frames_per_second = (float)json_object_get_number(frame, "fps");
                if (sprite->frames_per_second < 0) {
                    throw(JsonException, "FPS cannot be less than 0");
                }
            } else {
                sprite->frames_per_second = default_fps;
            }

            if (json_object_has_value_of_type(frame, "startingframe", JSONNumber)) {
                sprite->starting_frame = (int)json_object_get_number(frame, "startingframe");
            } else if (json_object_has_value_of_type(frame, "starting_frame", JSONNumber)) {
                sprite->starting_frame = (int)json_object_get_number(frame, "starting_frame");
            } else {
                sprite->starting_frame = default_starting_frame;
            }

            if (sprite->starting_frame < 0) {
                throw(JsonException, "Starting Frame cannot be less than 0");
            }

            const char* texture_name = default_texture;
            if (json_object_has_value_of_type(frame, "texture", JSONString)) {
                texture_name = json_object_get_string(frame, "texture");
            }

            SDL_Texture* texture = resource_load_texture(renderer, texture_name);
            sprite->texture = texture;

            SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
            sprite->texel_width = 1 / (float)texture_width;
            sprite->texel_height = 1 / (float)texture_height;

            if (json_object_has_value_of_type(frame, "frames", JSONArray)) {
                float width = default_width;
                float height = default_height;
                Padding padding = default_padding;
                const char* texture = default_texture;

                if (json_object_has_value_of_type(frame, "width", JSONNumber)) {
                    width = (float)json_object_get_number(frame, "width");
                }
                
                if (json_object_has_value_of_type(frame, "height", JSONNumber)) {
                    height = (float)json_object_get_number(frame, "height");
                }

                if (json_object_has_value(frame, "padding")) {
                    padding = soren_json_parse_padding(json_object_get_value(frame, "padding"));
                }

                JSON_Array* frame_array = json_object_get_array(frame, "frames");
                int frame_array_count = json_array_get_count(frame_array);
                for (int j = 0; j < frame_array_count; j++) {
                    JSON_Value* rect_element = json_array_get_value(frame_array, j);
                    if (json_value_get_type(rect_element) != JSONObject) {
                        throw(JsonException, "Expected a frame object in the sprite frames array");
                    }

                    RectF bounds = sprite_atlas_json_read_frame(json_object(rect_element), width, height);
                    soren_sprite_frame_list_add(&sprite->frames, (SpriteFrame){ bounds, NULL });
                }
            } else {
                RectF bounds = sprite_atlas_json_read_frame(frame, default_width, default_height);
                soren_sprite_frame_list_add(&sprite->frames, (SpriteFrame){ bounds, NULL });
            }
        }
    } else if (json_object_has_value_of_type(root, "frames", JSONArray)) {
        Sprite* sprite = soren_malloc(sizeof(*sprite));
        soren_sprite_frame_list_init(&sprite->frames);

        JSON_Array* frame_array = json_object_get_array(root, "frames");
        int frame_array_count = json_array_get_count(frame_array);

        sprite->frames_per_second = default_fps;
        string_init(&sprite->name, "default");
        sprite->origin = default_origin;
        sprite->update_mode = default_update_mode;
        sprite->starting_frame = default_starting_frame;
        sprite->texture = resource_load_texture(renderer, default_texture);        

        SDL_QueryTexture(sprite->texture, NULL, NULL, &texture_width, &texture_height);
        sprite->texel_width = 1 / (float)texture_width;
        sprite->texel_height = 1 / (float)texture_height;

        for (int i = 0; i < frame_array_count; i++) {
            JSON_Value* rect_element = json_array_get_value(frame_array, i);
            if (json_value_get_type(rect_element) != JSONObject) {
                throw(JsonException, "Expected a frame object in the sprite frames array");
            }

            RectF bounds = sprite_atlas_json_read_frame(json_object(rect_element), default_width, default_height);
            soren_sprite_frame_list_add(&sprite->frames, (SpriteFrame){ bounds, NULL });(&sprite->frames, bounds);
        }

        sprite_map_add(atlas->animations, string_data(&sprite->name), sprite);
        sprite_list_add(atlas->animation_list, sprite);
    } else {
        Sprite* sprite = soren_malloc(sizeof(*sprite));
        soren_sprite_frame_list_init(&sprite->frames);

        RectF bounds = sprite_atlas_json_read_frame(root, default_width, default_height);
        soren_sprite_frame_list_add(&sprite->frames, (SpriteFrame){ bounds, NULL });

        sprite->frames_per_second = default_fps;
        string_init(&sprite->name, "default");
        sprite->origin = default_origin;
        sprite->update_mode = default_update_mode;
        sprite->starting_frame = default_starting_frame;
        sprite->texture = resource_load_texture(renderer, default_texture);

        SDL_QueryTexture(sprite->texture, NULL, NULL, &texture_width, &texture_height);
        sprite->texel_width = 1 / (float)texture_width;
        sprite->texel_height = 1 / (float)texture_height;

        sprite_map_add(atlas->animations, string_data(&sprite->name), sprite);
        sprite_list_add(atlas->animation_list, sprite);
    }
}

SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_json_file(SDL_Renderer* renderer, const char* file) {
    SpriteAtlas* atlas = soren_malloc(sizeof(*atlas));
    sprite_atlas_init_from_json_file(atlas, renderer, file);
    return atlas;
}

SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_json_string(SDL_Renderer* renderer, const char* json) {
    SpriteAtlas* atlas = soren_malloc(sizeof(*atlas));
    sprite_atlas_init_from_json_string(atlas, renderer, json);
    return atlas;
}

SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_map(SpriteMap* sprites) {
    SpriteAtlas* atlas = soren_malloc(sizeof(*atlas));
    sprite_atlas_init_from_map(atlas, sprites);
    return atlas;
}

SOREN_EXPORT SpriteAtlas* sprite_atlas_create_from_list(SpriteList* sprites) {
    SpriteAtlas* atlas = soren_malloc(sizeof(*atlas));
    sprite_atlas_init_from_list(atlas, sprites);
    return atlas;
}

SOREN_EXPORT void sprite_atlas_init_from_json_file(SpriteAtlas* atlas, SDL_Renderer* renderer, const char* file) {
    volatile bool case_insensitive = json_get_case_insensitive();
    json_set_case_insensitive(true);

    try {
        JSON_Value* value = json_parse_file(file);
        if (!value || json_value_get_type(value) != JSONObject) {
            throw(JsonException, "Failed to parse SpriteAtlas JSON file");
        }

        sprite_atlas_init_impl(atlas, renderer, json_object(value));

        json_value_free(value);
    } finally {
        json_set_case_insensitive(case_insensitive);
    }
}

SOREN_EXPORT void sprite_atlas_init_from_json_string(SpriteAtlas* atlas, SDL_Renderer* renderer, const char* json) {
    volatile bool case_insensitive = json_get_case_insensitive();
    json_set_case_insensitive(true);

    try {
        JSON_Value* value = json_parse_string(json);
        if (!value || json_value_get_type(value) != JSONObject) {
            throw(JsonException, "Failed to parse SpriteAtlas JSON string");
        }

        sprite_atlas_init_impl(atlas, renderer, json_object(value));

        json_value_free(value);
    } finally {
        json_set_case_insensitive(case_insensitive);
    }
}

SOREN_EXPORT void sprite_atlas_init_from_map(SpriteAtlas* atlas, SpriteMap* sprites) {
    const char* key;
    Sprite* sprite;

    atlas->animation_list = sprite_list_create();
    atlas->animations = sprites;

    map_iter_start(sprites, key, sprite) {
        if (strcmp(key, "default") == 0) {
            sprite_list_insert(atlas->animation_list, 0, sprite);
        } else {
            sprite_list_add(atlas->animation_list, sprite);
        }
    }
    map_iter_end
}

SOREN_EXPORT void sprite_atlas_init_from_list(SpriteAtlas* atlas, SpriteList* sprites) {
    Sprite* sprite;

    atlas->animation_list = sprites;
    atlas->animations = sprite_map_create();

    list_iter_start(sprites, sprite) {
        sprite_map_add(atlas->animations, string_data(&sprite->name), sprite);
    }
    list_iter_end
}

SOREN_EXPORT void sprite_atlas_free_resources(SpriteAtlas* atlas) {
    Sprite* sprite;
    list_iter_start(atlas->animation_list, sprite) {
        resource_decrement(sprite->texture);
        soren_sprite_frame_list_free(&sprite->frames);
        string_free_resources(&sprite->name);
        soren_free(sprite);
    }
    list_iter_end
    
    string_free_resources(&atlas->name);
    sprite_list_free(atlas->animation_list);
    sprite_map_free(atlas->animations);
}

SOREN_EXPORT void sprite_atlas_free(SpriteAtlas* atlas) {
    sprite_atlas_free_resources(atlas);
    soren_free(atlas);
}