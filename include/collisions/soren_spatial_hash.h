#ifndef SOREN_COLLISIONS_SOREN_SPATIAL_HASH_H
#define SOREN_COLLISIONS_SOREN_SPATIAL_HASH_H

#include "soren_colliders.h"
#include <generic_hybrid_set.h>

HYBRID_SET_DEFINE_H(ColliderCollection, collider_collection, Collider*)

typedef struct SpatialHash SpatialHash;
typedef bool (*ColliderPredicate)(Collider* collider, void* ctx);
typedef bool (*ColliderVectorTest)(Collider* collider, Vector vector, void* ctx);
typedef bool (*ColliderColliderTest)(Collider* collider, Collider* text, void* ctx);
typedef bool (*ColliderRectFTest)(Collider* collider, RectF rect, void* ctx);

// Collection functions

SOREN_EXPORT SpatialHash* spatial_hash_create(float cell_size);
SOREN_EXPORT void spatial_hash_free(SpatialHash* hash);

SOREN_EXPORT void spatial_hash_add(SpatialHash* hash, Collider* collider);

SOREN_EXPORT void spatial_hash_remove(SpatialHash* hash, Collider* collider);
SOREN_EXPORT void spatial_hash_remove_with_brute_force(SpatialHash* hash, Collider* collider);

SOREN_EXPORT void spatial_hash_clear(SpatialHash* hash);

// Movement functions

SOREN_EXPORT void spatial_hash_move(SpatialHash* hash, Collider* collider, Vector delta);
SOREN_EXPORT void spatial_hash_set_position(SpatialHash* hash, Collider* collider, Vector position);

SOREN_EXPORT void spatial_hash_rotate(SpatialHash* hash, Collider* collider, float delta_rotation);
SOREN_EXPORT void spatial_hash_set_rotation(SpatialHash* hash, Collider* collider, float rotation);

// Collision checking

SOREN_EXPORT ColliderCollection* spatial_hash_all(SpatialHash* hash, ColliderCollection* results);
SOREN_EXPORT ColliderCollection* spatial_hash_all_ext(SpatialHash* hash, ColliderCollection* results, void* ctx, ColliderPredicate predicate);

SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_vector(SpatialHash* hash, Vector position, ColliderCollection* results);
SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_rectf(SpatialHash* hash, RectF rect, ColliderCollection* results);
SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_collider(SpatialHash* hash, Collider* collider, ColliderCollection* results);

SOREN_EXPORT bool spatial_hash_collides_vector(SpatialHash* hash, Vector position);
SOREN_EXPORT bool spatial_hash_collides_vector_ext(SpatialHash* hash, Vector position, void* ctx, ColliderVectorTest test);

SOREN_EXPORT bool spatial_hash_collides_rectf(SpatialHash* hash, RectF bounds);
SOREN_EXPORT bool spatial_hash_collides_rectf_ext(SpatialHash* hash, RectF bounds, void* ctx, ColliderRectFTest test);

SOREN_EXPORT bool spatial_hash_collides_collider(SpatialHash* hash, Collider* collider);
SOREN_EXPORT bool spatial_hash_collides_collider_ext(SpatialHash* hash, Collider* collider, void* ctx, ColliderColliderTest test);

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_vector(SpatialHash* hash, ColliderCollection* results, Vector position);
SOREN_EXPORT ColliderCollection* spatial_hash_collisions_vector_ext(SpatialHash* hash, ColliderCollection* results, Vector position, void* ctx, ColliderVectorTest test);

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_rectf(SpatialHash* hash, ColliderCollection* results, RectF bounds);
SOREN_EXPORT ColliderCollection* spatial_hash_collisions_rectf_ext(SpatialHash* hash, ColliderCollection* results, RectF bounds, void* ctx, ColliderRectFTest test);

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_collider(SpatialHash* hash, ColliderCollection* results, Collider* collider);
SOREN_EXPORT ColliderCollection* spatial_hash_collisions_collider_ext(SpatialHash* hash, ColliderCollection* results, Collider* collider, void* ctx, ColliderColliderTest test);

SOREN_EXPORT Collider* spatial_hash_first_vector(SpatialHash* hash, Vector position);
SOREN_EXPORT Collider* spatial_hash_first_vector_ext(SpatialHash* hash, Vector position, void* ctx, ColliderVectorTest test);

SOREN_EXPORT Collider* spatial_hash_first_rectf(SpatialHash* hash, RectF bounds);
SOREN_EXPORT Collider* spatial_hash_first_rectf_ext(SpatialHash* hash, RectF bounds, void* ctx, ColliderRectFTest test);

SOREN_EXPORT Collider* spatial_hash_first_collider(SpatialHash* hash, Collider* collider);
SOREN_EXPORT Collider* spatial_hash_first_collider_ext(SpatialHash* hash, Collider* collider, void* ctx, ColliderColliderTest test);

#endif