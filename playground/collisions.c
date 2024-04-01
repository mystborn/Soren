#include "run.h"

#include <collisions/soren_spatial_hash.h>
#include <collisions/soren_collisions.h>
#include <soren_input.h>
#include <graphics/soren_graphics.h>
#include <generic_iterators/hybrid_set_iterator.h>
#include <graphics/soren_primitives.h>

char* title = "Collisions Playground";

static SpatialHash* hash;
static Collider* player;
static Collider* colliders[3];

void game_init(SDL_Window* window, SDL_Renderer* renderer) {
    hash = spatial_hash_create(64);
    player = box_collider_create(32, 32);

    // box_collider_set_original_center((BoxCollider*)player, vector_create(16, 16));
    collider_set_rotation(player, 3);
    collider_set_position(player, vector_create(40, 64));

    BoxCollider* wall1 = box_collider_create(16, 200);
    collider_set_position(wall1, vector_create(16, 16));

    BoxCollider* wall2 = box_collider_create(200, 32);
    collider_set_position(wall2, vector_create(32, 168));

    colliders[0] = wall1;
    colliders[1] = wall2;
    colliders[2] = player;

    spatial_hash_add(hash, player);
    spatial_hash_add(hash, wall1);
    spatial_hash_add(hash, wall2);
}

void game_update(SDL_Window* window, SDL_Renderer* renderer, float delta) {
    draw_line_color(
        renderer,
        vector_create(0, 64),
        vector_create(450, 64),
        2,
        soren_colors.purple);

    int left = key_check(SDL_SCANCODE_A);
    int right = key_check(SDL_SCANCODE_D);
    int up = key_check(SDL_SCANCODE_W);
    int down = key_check(SDL_SCANCODE_S);
    int rotate_left = key_check(SDL_SCANCODE_Q);
    int rotate_right = key_check(SDL_SCANCODE_E);
    int rotate_reset = key_check_pressed(SDL_SCANCODE_Z);

    float x = (right - left) * 4 * 60 * delta;
    float y = (down - up) * 4 * 60 * delta;
    float rotation = (rotate_right - rotate_left) * 4 * 60 * delta;
    bool first_frame = true;

    if (x != 0 || y != 0 || rotation != 0 || first_frame) {
        first_frame = false;
        Vector delta = vector_create(x, y);
        Vector new_position = vector_add(collider_position(player), delta);
        collider_set_position(player, new_position);

        rotation = collider_rotation(player) + degrees_to_radians(rotation);
        if (rotate_reset)
            rotation = 0;

        collider_set_rotation(player, rotation);

        ColliderCollection* collisions = spatial_hash_collisions_collider(hash, NULL, player);

        if (collider_collection_count(collisions) > 0) {
            log_trace(soren_logger, "Collisions: %d", collider_collection_count(collisions));
            Collider* other;
            CollisionResult cr;
            RaycastHit hit;
            hybrid_set_iter_start(collisions, other) {
                collider_collides_collider_impl(player, other, &cr, &hit);
                Vector mtv;
                if (!vector_equals(cr.minimum_translation_vector, VECTOR_ZERO)) {
                    mtv = cr.minimum_translation_vector;
                } else if (hit.distance != 0) {
                    mtv = hit.point;
                }

                spatial_hash_move(hash, player, vector_negate(mtv));
            }
            hybrid_set_iter_end
        }
    }

    for (int i = 0; i < 3; i++) {
        SDL_FColor color = (i == 2 ? soren_colors.blue : soren_colors.red);
        collider_debug_draw_impl(colliders[i], renderer, color);
    }
}