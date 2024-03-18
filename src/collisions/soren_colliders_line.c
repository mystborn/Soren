#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>
#include <graphics/soren_primitives.h>

#include "soren_collisions_shared.h"

static void line_collider_clean(LineCollider* line) {
    line->dirty = false;
    Vector start = vector_multiply_scalar(line->original_start, line->scale);
    Vector end = vector_multiply_scalar(line->original_end, line->scale);
    Vector pivot = vector_multiply_scalar(line->original_pivot, line->scale);

    if (line->rotation != 0) {
        float sin = SDL_sinf(line->rotation);
        float cos = SDL_cosf(line->rotation);

        start = vector_subtract(start, pivot);
        start = vector_create(start.x * cos - start.y * sin, start.x * sin + start.y * cos);
        start = vector_add(start, pivot);

        end = vector_subtract(end, pivot);
        end = vector_create(end.x * cos - end.y * sin, end.x * sin + end.y * cos);
        end = vector_add(end, pivot);
    }

    line->start = start;
    line->end = end;
    line->pivot = pivot;

    Vector top_left = vector_min(start, end);
    Vector bottom_right = vector_max(start, end);

    line->bounding_box = (RectF) {
        top_left.x,
        top_left.y,
        bottom_right.x - top_left.x,
        bottom_right.y - top_left.y
    };
}

SOREN_EXPORT float line_collider_rotation(LineCollider* line) {
    return line->rotation;
}

SOREN_EXPORT void line_collider_set_rotation(LineCollider* line, float rotation) {
    if (rotation == line->rotation)
        return;

    line->rotation = rotation;
    line->dirty = true;
}

SOREN_EXPORT float line_collider_scale(LineCollider* line) {
    return line->scale;
}

SOREN_EXPORT void line_collider_set_scale(LineCollider* line, float scale) {
    if (scale == line->scale)
        return;

    line->scale = scale;
    line->dirty = true;
}

SOREN_EXPORT Vector line_collider_position(LineCollider* line) {
    return line->position;
}

SOREN_EXPORT void line_collider_set_position(LineCollider* line, Vector position) {
    if (vector_equals(position, line->position))
        return;

    line->position = position;
    line->dirty = true;
}

SOREN_EXPORT RectF line_collider_bounds(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    RectF bounds = line->bounding_box;
    bounds.x += line->position.x;
    bounds.y += line->position.y;
    return bounds;
}

SOREN_EXPORT void line_collider_debug_draw(LineCollider* line, SDL_Renderer* renderer, SDL_FColor color) {
    draw_line_color(
        renderer,
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        line->scale,
        color);
}

SOREN_EXPORT Vector line_collider_start(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->start;
}

SOREN_EXPORT Vector line_collider_end(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->end;
}

SOREN_EXPORT Vector line_collider_pivot(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->pivot;
}

SOREN_EXPORT Vector line_collider_adjusted_start(LineCollider* line) {
    Vector start = line_collider_start(line);
    return vector_add(start, line->position);
}

SOREN_EXPORT Vector line_collider_adjusted_end(LineCollider* line) {
    Vector end = line_collider_end(line);
    return vector_add(end, line->position);
}

SOREN_EXPORT Vector line_collider_original_start(LineCollider* line) {
    return line->original_start;
}

SOREN_EXPORT void line_collider_set_original_start(LineCollider* line, Vector start) {
    if (vector_equals(start, line->original_start))
        return;

    line->original_start = start;
    line->dirty = true;
}

SOREN_EXPORT Vector line_collider_original_end(LineCollider* line) {
    return line->original_end;
}

SOREN_EXPORT void line_collider_set_original_end(LineCollider* line, Vector end) {
    if (vector_equals(end, line->original_end))
        return;

    line->original_end = end;
    line->dirty = true;
}

SOREN_EXPORT Vector line_collider_original_pivot(LineCollider* line) {
    return line->original_pivot;
}

SOREN_EXPORT void line_collider_set_original_pivot(LineCollider* line, Vector pivot) {
    if (vector_equals(pivot, line->original_pivot))
        return;

    line->original_pivot = pivot;
    line->dirty = true;
}

SOREN_EXPORT bool line_collider_overlaps_rect(LineCollider* collider, RectF rect) {
    BoxCollider* box = collision_shared_box_collider_init(rect);
    return collision_line_to_poly(collider, (PolygonCollider*)box);
}

SOREN_EXPORT bool line_collider_overlaps_collider(LineCollider* collider, Collider* other) {
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return line_collider_overlaps_collider(collider, (Collider*)point->box);
            } else {
                return collision_point_to_line(collider_position(point), collider);
            }
        case COLLIDER_LINE:
            return collision_line_to_line(collider, (LineCollider*)other);
        case COLLIDER_CIRCLE:
            return collision_line_to_circle(collider, (CircleCollider*)other);
        case COLLIDER_BOX:
        case COLLIDER_POLYGON:
            return collision_line_to_poly(collider, (PolygonCollider*)other);
        default:
            throw(InvalidColliderType, "Invalid collider for line overlap check.");
            break;
    }

    return false;
}

SOREN_EXPORT bool line_collider_overlaps_line(LineCollider* collider, Vector start, Vector end) {
    return collision_line_to_segment(collider, start, end);
}

SOREN_EXPORT bool line_collider_contains_point(LineCollider* collider, Vector point) {
    return collision_point_to_line(point, collider);
}

SOREN_EXPORT bool line_collider_collides_rect(LineCollider* collider, RectF rect, CollisionResult* out_result) {
    RaycastHit hit = (RaycastHit){0};
    BoxCollider* box = collision_shared_box_collider_init(rect);
    bool result = collision_line_to_poly_ext(collider, (PolygonCollider*)box, &hit);
    if (result && out_result) {
        raycast_hit_to_collision_result(&hit, out_result);
    }

    return result;
}

SOREN_EXPORT bool line_collider_collides_collider(LineCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return line_collider_collides_collider(collider, (Collider*)point->box, out_result, out_hit);
            } else {
                return collision_point_to_line_ext(collider_position(point), collider, out_result);
            }
        case COLLIDER_LINE:
            return collision_line_to_line_ext(collider, (LineCollider*)other, out_result);
        case COLLIDER_CIRCLE:
            return collision_line_to_circle_ext(collider, (CircleCollider*)other, out_hit);
        case COLLIDER_BOX:
        case COLLIDER_POLYGON:
            return collision_line_to_poly_ext(collider, (PolygonCollider*)other, out_hit);
        default:
            throw(InvalidColliderType, "Invalid collider for line collides check.");
            break;
    }

    return false;
}

SOREN_EXPORT bool line_collider_collides_line(LineCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    CollisionResult collision_result = (CollisionResult){0};
    bool result = collision_line_to_segment_ext(collider, start, end, &collision_result);
    if (result && out_result) {
        collision_result_to_raycast_hit(&collision_result, out_result);
    }

    return result;
}

SOREN_EXPORT bool line_collider_collides_point(LineCollider* collider, Vector point, CollisionResult* out_result) {
    return collision_point_to_line_ext(point, collider, out_result);
}