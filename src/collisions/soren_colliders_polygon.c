#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>

#include <float.h>

#include "soren_collisions_shared.h"


PolygonCollider* polygon_collider_create(Vector* points, int count) {
    PolygonCollider* collider = soren_malloc(sizeof(*collider));
    if (!collider)
        return NULL;

    polygon_collider_init(collider, points, count);
    return collider;
}

void polygon_collider_init(PolygonCollider* polygon, Vector* points, int count) {
    assert(polygon);
    collider_init((Collider*)polygon, COLLIDER_POLYGON);
    polygon->bounding_box = RECTF_EMPTY;
    polygon->position = VECTOR_ZERO;
    polygon->original_center = VECTOR_ZERO;
    polygon->scale = 1;
    polygon->rotation = 0;
    polygon->dirty = true;
    polygon->points_count = count;

    polygon->edge_normals = soren_malloc(count * sizeof(*polygon->edge_normals));
    polygon->points = soren_malloc(count * sizeof(*polygon->points));
    polygon->original_points = soren_malloc(count * sizeof(*polygon->original_points));

    SDL_memcpy(polygon->original_points, points, count * sizeof(*points));
}

void polygon_collider_free_resources(PolygonCollider* polygon) {
    soren_free(polygon->edge_normals);
    soren_free(polygon->points);
    soren_free(polygon->original_points);
}

static void polygon_set_edge_normal(PolygonCollider* polygon, int index) {
    if (index == 0)
        return;

    Vector perp = vector_perpendicular(
        polygon->points[index - 1],
        polygon->points[index]);
    
    perp = vector_normalize(perp);

    polygon->edge_normals[index - 1] = perp;

    if (index == polygon->points_count - 1) {
        perp = vector_perpendicular(polygon->points[index], polygon->points[0]);
        perp = vector_normalize(perp);
        polygon->edge_normals[index] = perp;
    }
}

static void polygon_clean(PolygonCollider* polygon) {
    if (!polygon->dirty)
        return;

    polygon->dirty = false;
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = FLT_MIN;
    float max_y = FLT_MIN;

    // There are two branches to calculate the final points of the polygon.
    // The first is used if the poly has been rotated, and includes all
    // transformations. The second eschews the unnecessary rotation transforms
    // for efficiency.
    if (polygon->rotation != 0) {
        float sin = SDL_sinf(polygon->rotation);
        float cos = SDL_cosf(polygon->rotation);
        Vector center = polygon_collider_center(polygon);

        for (int i = 0; i < polygon->points_count; i++) {
            Vector p = polygon->original_points[i];
            p = vector_multiply_scalar(p, polygon->scale);
            p = vector_subtract(p, center);
            p = vector_create(p.x * cos - p.y * sin, p.x * sin + p.y * cos);
            p = vector_add(p, center);

            if (p.x < min_x)
                min_x = p.x;
            if (p.x > max_x)
                max_x = p.x;
            if (p.y < min_y)
                min_y = p.y;
            if (p.y > max_y)
                max_y = p.y;

            polygon->points[i] = p;

            polygon_set_edge_normal(polygon, i);
        }
    } else {
        for (int i = 0; i < polygon->points_count; i++) {
            Vector p = polygon->original_points[i];
            p = vector_multiply_scalar(p, polygon->scale);

            if (p.x < min_x)
                min_x = p.x;
            if (p.x > max_x)
                max_x = p.x;
            if (p.y < min_y)
                min_y = p.y;
            if (p.y > max_y)
                max_y = p.y;

            polygon->points[i] = p;

            polygon_set_edge_normal(polygon, i);
        }
    }

    polygon->bounding_box = (RectF) {
        min_x,
        min_y,
        max_x - min_x,
        max_y - min_y
    };
}

float polygon_collider_rotation(PolygonCollider* polygon) {
    return polygon->rotation;
}

void polygon_collider_set_rotation(PolygonCollider* polygon, float rotation) {
    if (rotation == polygon->rotation)
        return;
    
    polygon->rotation = rotation;
    polygon->dirty = true;
}

float polygon_collider_scale(PolygonCollider* polygon) {
    return polygon->scale;
}

void polygon_collider_set_scale(PolygonCollider* polygon, float scale) {
    if (scale == polygon->scale)
        return;

    collider_assert_scale(scale);
    polygon->scale = scale;
    polygon->dirty = true;
}

Vector polygon_collider_position(PolygonCollider* polygon) {
    return polygon->position;
}

void polygon_collider_set_position(PolygonCollider* polygon, Vector position) {
    polygon->position = position;
}

Vector polygon_collider_center(PolygonCollider* polygon) {
    Vector center = polygon->original_center;
    center.x *= polygon->scale;
    center.y *= polygon->scale;

    return center;
}

Vector polygon_collider_original_center(PolygonCollider* polygon) {
    return polygon->original_center;
}

void polygon_collider_set_original_center(PolygonCollider* polygon, Vector center) {
    polygon->original_center = center;
    polygon->dirty = true;
}

Vector* polygon_collider_points(PolygonCollider* polygon, int* out_count) {
    polygon_clean(polygon);

    if (out_count)
        *out_count = polygon->points_count;

    return polygon->points;
}

RectF polygon_collider_bounds(PolygonCollider* polygon) {
    polygon_clean(polygon);
    Vector center = polygon_collider_center(polygon);

    return (RectF){
        .x = polygon->bounding_box.x + polygon->position.x - center.x,
        .y = polygon->bounding_box.y + polygon->position.y - center.y,
        .w = polygon->bounding_box.w,
        .h = polygon->bounding_box.h
    };
}

Vector* polygon_collider_edge_normals(PolygonCollider* polygon, int* out_count) {
    polygon_clean(polygon);

    if (out_count)
        *out_count = polygon->points_count;

    return polygon->edge_normals;
}

bool polygon_collider_overlaps_rect(PolygonCollider* collider, RectF rect) {
    BoxCollider* box = collision_shared_box_collider_init(rect);
    return collision_polygon_to_polygon(collider, (PolygonCollider*)box);
}

bool polygon_collider_overlaps_collider(PolygonCollider* collider, Collider* other) {
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return polygon_collider_overlaps_collider(collider, (Collider*)point->box);
            } else {
                return polygon_collider_contains_point(collider, point_collider_position(point));
            }
        case COLLIDER_LINE:
            return collision_line_to_poly((LineCollider*)other, collider);
        case COLLIDER_CIRCLE:
            return collision_circle_to_polygon((CircleCollider*)other, collider);
        case COLLIDER_BOX:
        case COLLIDER_POLYGON:
            return collision_polygon_to_polygon(collider, (PolygonCollider*)other);
        default:
            throw(InvalidColliderType, "Invalid collider for polygon overlap check.");
            break;
    }

    return false;
}

bool polygon_collider_overlaps_line(PolygonCollider* collider, Vector start, Vector end) {
    return collision_segment_to_poly(start, end, collider);
}

bool polygon_collider_contains_point(PolygonCollider* collider, Vector point) {
    return collision_point_to_poly(point, collider);
}

bool polygon_collider_collides_rect(PolygonCollider* collider, RectF rect, CollisionResult* out_result) {
    BoxCollider* box = collision_shared_box_collider_init(rect);
    return collision_polygon_to_polygon_ext(collider, (PolygonCollider*)box, out_result);
}

bool polygon_collider_collides_collider(PolygonCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return polygon_collider_collides_collider(collider, (Collider*)point->box, out_result, out_hit);
            } else {
                return polygon_collider_collides_point(collider, point_collider_position(point), out_result);
            }
        case COLLIDER_LINE:
            return collision_line_to_poly_ext((LineCollider*)other, collider, out_hit);
        case COLLIDER_CIRCLE:
            bool result = collision_circle_to_polygon_ext((CircleCollider*)other, collider, out_result);
            if (result && out_result) {
                collision_result_invert(out_result);
            }

            return result;
        case COLLIDER_BOX:
        case COLLIDER_POLYGON:
            return collision_polygon_to_polygon_ext(collider, (PolygonCollider*)other, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider for polygon collides check.");
            break;
    }

    return false;
}
bool polygon_collider_collides_line(PolygonCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    return collision_segment_to_poly_ext(start, end, collider, out_result);
}

bool polygon_collider_collides_point(PolygonCollider* collider, Vector point, CollisionResult* out_result) {
    bool result = collision_point_to_poly_ext(point, collider, out_result);
    if (result && out_result) {
        collision_result_invert(out_result);
    }

    return result;
}