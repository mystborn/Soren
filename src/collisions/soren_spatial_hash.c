#include <collisions/soren_spatial_hash.h>

#include <generic_map.h>
#include <generic_iterators/map_iterator.h>
#include <generic_iterators/set_iterator.h>
#include <generic_iterators/list_iterator.h>

HYBRID_SET_DEFINE_C(ColliderCollection, collider_collection, Collider*, gds_pointer_hash, gds_pointer_compare)

MAP_DEFINE_H(SpatialStore, spatial_store, Point, ColliderCollection*)
MAP_DEFINE_C(SpatialStore, spatial_store, Point, ColliderCollection*, point_hash, point_compare)

struct SpatialHash {
    float inverse_cell_size;
    SpatialStore* cells;
    ColliderCollection* cache;
    ColliderCollection* secondary_cache;
};

static inline int fast_floor(float x) {
    return (int)(x + 32768) - 32768;
}

#define GET_OVERLAPPING_SET_START(rect, spatial_hash) \
    int minx = fast_floor(rectf_left(rect) * spatial_hash->inverse_cell_size); \
    int miny = fast_floor(rectf_top(rect) * spatial_hash->inverse_cell_size); \
    int maxx = fast_floor(rectf_right(rect) * spatial_hash->inverse_cell_size) + 1; \
    int maxy = fast_floor(rectf_bottom(rect) * spatial_hash->inverse_cell_size) + 1; \
    Point p; \
    ColliderCollection* set; \
    for (int w = minx; w < maxx; w++) { \
        for (int h = miny; h < maxy; h++) { \
            p.x = w; \
            p.y = h; \
            if(!spatial_store_try_get(spatial_hash->cells, p, &set)) { \
                set = collider_collection_create(); \
                spatial_store_add(spatial_hash->cells, p, set); \
            } \

#define GET_OVERLAPPING_SET_END } }

static inline RectF vector_to_rectf(Vector v) {
    return (RectF){ v.x, v.y, 0, 0 };
}

static inline Point vector_to_cell_point(SpatialHash* hash, Vector v) {
    return (Point){ fast_floor(v.x * hash->inverse_cell_size), fast_floor(v.y * hash->inverse_cell_size) };
}

SOREN_EXPORT SpatialHash* spatial_hash_create(float cell_size) {
    SpatialHash* result = soren_malloc(sizeof(*result));
    result->inverse_cell_size = 1.f / cell_size;
    result->cells = spatial_store_create();
    result->cache = collider_collection_create();
    result->secondary_cache = collider_collection_create();

    return result;
}

SOREN_EXPORT void spatial_hash_free(SpatialHash* hash) {
    ColliderCollection* collection;
    map_iter_value_start(hash->cells, collection) {
        collider_collection_free(collection);
    }
    map_iter_end

    collider_collection_free(hash->cache);
    collider_collection_free(hash->secondary_cache);
    spatial_store_free(hash->cells);

    soren_free(hash);
}

SOREN_EXPORT void spatial_hash_add(SpatialHash* hash, Collider* collider) {
    RectF bounds = collider_bounds(collider);

    GET_OVERLAPPING_SET_START(bounds, hash) {
        collider_collection_add(set, collider);
    }
    GET_OVERLAPPING_SET_END
}

SOREN_EXPORT void spatial_hash_clear(SpatialHash* hash) {
    ColliderCollection* set;

    map_iter_value_start(hash->cells, set) {
        collider_collection_free(set);
    }
    map_iter_end

    spatial_store_clear(hash->cells, true);
}

SOREN_EXPORT void spatial_hash_remove(SpatialHash* hash, Collider* collider) {
    RectF bounds = collider_bounds(collider);

    GET_OVERLAPPING_SET_START(bounds, hash)

    collider_collection_remove(set, collider);

    GET_OVERLAPPING_SET_END
}

SOREN_EXPORT void spatial_hash_remove_with_brute_force(SpatialHash* hash, Collider* collider) {
    ColliderCollection* set;

    map_iter_value_start(hash->cells, set) {
        collider_collection_remove(set, collider);
    }
    map_iter_end
}

SOREN_EXPORT void spatial_hash_move(SpatialHash* hash, Collider* collider, Vector delta) {
    spatial_hash_remove(hash, collider);
    collider_set_position(collider, vector_add(delta, collider_position(collider)));
    spatial_hash_add(hash, collider);
}

SOREN_EXPORT void spatial_hash_set_position(SpatialHash* hash, Collider* collider, Vector position) {
    spatial_hash_remove(hash, collider);
    collider_set_position(collider, position);
    spatial_hash_add(hash, collider);
}

SOREN_EXPORT void spatial_hash_rotate(SpatialHash* hash, Collider* collider, float delta_rotation) {
    spatial_hash_remove(hash, collider);
    collider_set_rotation(collider, delta_rotation + collider_rotation(collider));
    spatial_hash_add(hash, collider);
}

SOREN_EXPORT void spatial_hash_set_rotation(SpatialHash* hash, Collider* collider, float rotation) {
    spatial_hash_remove(hash, collider);
    collider_set_rotation(collider, rotation);
    spatial_hash_add(hash, collider);
}

SOREN_EXPORT ColliderCollection* spatial_hash_all(SpatialHash* hash, ColliderCollection* results) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    ColliderCollection* set;

    map_iter_value_start(hash->cells, set) {
        collider_collection_union(results, set, results);
    }
    map_iter_end

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_all_ext(SpatialHash* hash, ColliderCollection* results, void* ctx, ColliderPredicate predicate) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    ColliderCollection* set;
    Collider* collider;

    map_iter_value_start(hash->cells, set) {
        if (set->using_set) {
            set_iter_start(set->set, collider) {
                if (predicate(collider, ctx)) {
                    collider_collection_add(results, collider);
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, collider) {
                collider_collection_add(results, collider);
            }
            list_iter_end
        }
    }
    map_iter_end

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_vector(SpatialHash* hash, Vector position, ColliderCollection* results) {
    return spatial_hash_broadphase_rectf(hash, vector_to_rectf(position), results);
}

SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_rectf(SpatialHash* hash, RectF rect, ColliderCollection* results) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    GET_OVERLAPPING_SET_START(rect, hash)

    collider_collection_union(set, results, results);

    GET_OVERLAPPING_SET_END

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_broadphase_collider(SpatialHash* hash, Collider* collider, ColliderCollection* results) {
    return spatial_hash_broadphase_rectf(hash, collider_bounds(collider), results);
}
SOREN_EXPORT bool spatial_hash_collides_vector(SpatialHash* hash, Vector position) {
    return spatial_hash_first_vector(hash, position) != NULL;
}

SOREN_EXPORT bool spatial_hash_collides_vector_ext(SpatialHash* hash, Vector position, void* ctx, ColliderVectorTest test) {
    return spatial_hash_first_vector_ext(hash, position, ctx, test) != NULL;
}

SOREN_EXPORT bool spatial_hash_collides_rectf(SpatialHash* hash, RectF bounds) {
    return spatial_hash_first_rectf(hash, bounds) != NULL;
}

SOREN_EXPORT bool spatial_hash_collides_rectf_ext(SpatialHash* hash, RectF bounds, void* ctx, ColliderRectFTest test) {
    return spatial_hash_first_rectf_ext(hash, bounds, ctx, test) != NULL;
}

SOREN_EXPORT bool spatial_hash_collides_collider(SpatialHash* hash, Collider* collider) {
    return spatial_hash_first_collider(hash, collider) != NULL;
}

SOREN_EXPORT bool spatial_hash_collides_collider_ext(SpatialHash* hash, Collider* collider, void* ctx, ColliderColliderTest test) {
    return spatial_hash_first_collider_ext(hash, collider, ctx, test) != NULL;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_vector(SpatialHash* hash, ColliderCollection* results, Vector position) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_vector(hash, position, hash->secondary_cache);

    Collider* collider;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, collider) {
            if (collider_overlaps(collider, position)) {
                collider_collection_add(results, collider);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, collider) {
            if (collider_overlaps(collider, position)) {
                collider_collection_add(results, collider);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_vector_ext(SpatialHash* hash, ColliderCollection* results, Vector position, void* ctx, ColliderVectorTest test) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_vector(hash, position, hash->secondary_cache);

    Collider* collider;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, collider) {
            if (collider_overlaps(collider, position) && test(collider, position, ctx)) {
                collider_collection_add(results, collider);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, collider) {
            if (collider_overlaps(collider, position) && test(collider, position, ctx)) {
                collider_collection_add(results, collider);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_rectf(SpatialHash* hash, ColliderCollection* results, RectF bounds) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_rectf(hash, bounds, hash->secondary_cache);

    Collider* collider;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, collider) {
            if (collider_overlaps(collider, bounds)) {
                collider_collection_add(results, collider);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, collider) {
            if (collider_overlaps(collider, bounds)) {
                collider_collection_add(results, collider);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_rectf_ext(SpatialHash* hash, ColliderCollection* results, RectF bounds, void* ctx, ColliderRectFTest test) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_rectf(hash, bounds, hash->secondary_cache);

    Collider* collider;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, collider) {
            if (collider_overlaps(collider, bounds) && test(collider, bounds, ctx)) {
                collider_collection_add(results, collider);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, collider) {
            if (collider_overlaps(collider, bounds) && test(collider, bounds, ctx)) {
                collider_collection_add(results, collider);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_collider(SpatialHash* hash, ColliderCollection* results, Collider* collider) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_collider(hash, collider, hash->secondary_cache);

    Collider* other;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, other) {
            if (collider != other && collider_overlaps(collider, other)) {
                collider_collection_add(results, other);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, other) {
            if (collider != other && collider_overlaps(collider, other)) {
                collider_collection_add(results, other);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT ColliderCollection* spatial_hash_collisions_collider_ext(SpatialHash* hash, ColliderCollection* results, Collider* collider, void* ctx, ColliderColliderTest test) {
    if (!results) {
        collider_collection_clear(hash->cache);
        results = hash->cache;
    }

    collider_collection_clear(hash->secondary_cache);
    spatial_hash_broadphase_collider(hash, collider, hash->secondary_cache);

    Collider* other;

    if (hash->secondary_cache->using_set) {
        set_iter_start(hash->secondary_cache->set, other) {
            if (collider != other && collider_overlaps(collider, other) && test(collider, other, ctx)) {
                collider_collection_add(results, other);
            }
        } 
        set_iter_end
    } else {
        list_iter_start(hash->secondary_cache->list, other) {
            if (collider != other && collider_overlaps(collider, other) && test(collider, other, ctx)) {
                collider_collection_add(results, other);
            }
        }
        list_iter_end
    }

    return results;
}

SOREN_EXPORT Collider* spatial_hash_first_vector(SpatialHash* hash, Vector position) {
    Point p = vector_to_cell_point(hash, position);
    ColliderCollection* set;
    Collider* collider;

    if (spatial_store_try_get(hash->cells, p, &set)) {
        if (set->using_set) {
            set_iter_start(set->set, collider) {
                if (collider_contains_point_impl(collider, position)) {
                    return collider;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, collider) {
                if (collider_contains_point_impl(collider, position)) {
                    return collider;
                }
            }
            list_iter_end
        }
    }

    return NULL;
}

SOREN_EXPORT Collider* spatial_hash_first_vector_ext(SpatialHash* hash, Vector position, void* ctx, ColliderVectorTest test) {
    Point p = vector_to_cell_point(hash, position);
    ColliderCollection* set;
    Collider* collider;

    if (spatial_store_try_get(hash->cells, p, &set)) {
        if (set->using_set) {
            set_iter_start(set->set, collider) {
                if (collider_contains_point_impl(collider, position) && test(collider, position, ctx)) {
                    return collider;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, collider) {
                if (collider_contains_point_impl(collider, position) && test(collider, position, ctx)) {
                    return collider;
                }
            }
            list_iter_end
        }
    }

    return NULL;
}

SOREN_EXPORT Collider* spatial_hash_first_rectf(SpatialHash* hash, RectF bounds) {
    Collider* collider;

    GET_OVERLAPPING_SET_START(bounds, hash)

    if (set->using_set) {
            set_iter_start(set->set, collider) {
                if (collider_overlaps(collider, bounds)) {
                    return collider;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, collider) {
                if (collider_overlaps(collider, bounds)) {
                    return collider;
                }
            }
            list_iter_end
        }

    GET_OVERLAPPING_SET_END

    return NULL;
}

SOREN_EXPORT Collider* spatial_hash_first_rectf_ext(SpatialHash* hash, RectF bounds, void* ctx, ColliderRectFTest test) {
    Collider* collider;

    GET_OVERLAPPING_SET_START(bounds, hash)

    if (set->using_set) {
            set_iter_start(set->set, collider) {
                if (collider_overlaps(collider, bounds) && test(collider, bounds, ctx)) {
                    return collider;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, collider) {
                if (collider_overlaps(collider, bounds) && test(collider, bounds, ctx)) {
                    return collider;
                }
            }
            list_iter_end
        }

    GET_OVERLAPPING_SET_END

    return NULL;
}

SOREN_EXPORT Collider* spatial_hash_first_collider(SpatialHash* hash, Collider* collider) {
    Collider* other;
    RectF bounds = collider_bounds(collider);

    GET_OVERLAPPING_SET_START(bounds, hash)

    if (set->using_set) {
            set_iter_start(set->set, other) {
                if (other != collider && collider_overlaps(collider, other)) {
                    return other;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, other) {
                if (other != collider && collider_overlaps(collider, other)) {
                    return other;
                }
            }
            list_iter_end
        }

    GET_OVERLAPPING_SET_END

    return NULL;
}

SOREN_EXPORT Collider* spatial_hash_first_collider_ext(SpatialHash* hash, Collider* collider, void* ctx, ColliderColliderTest test) {
    Collider* other;
    RectF bounds = collider_bounds(collider);

    GET_OVERLAPPING_SET_START(bounds, hash)

    if (set->using_set) {
            set_iter_start(set->set, other) {
                if (other != collider && collider_overlaps(collider, other) && test(collider, other, ctx)) {
                    return other;
                }
            }
            set_iter_end
        } else {
            list_iter_start(set->list, other) {
                if (other != collider && collider_overlaps(collider, other) && test(collider, other, ctx)) {
                    return other;
                }
            }
            list_iter_end
        }

    GET_OVERLAPPING_SET_END

    return NULL;
}