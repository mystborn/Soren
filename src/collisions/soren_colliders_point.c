#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>
#include <graphics/soren_primitives.h>

#include "soren_collisions_shared.h"

void point_collider_free_resources(PointCollider* point) {
    if (point->box) {
        collider_free_resources((Collider*)point->box);
    }
}

SOREN_EXPORT float point_collider_rotation(PointCollider* point) {
    return point->rotation;
}

SOREN_EXPORT void point_collider_set_rotation(PointCollider* point, float rotation) {
    if (rotation == point->rotation) {
        return;
    }

    point->rotation = rotation;
    if (point->scale != 1) {
        box_collider_set_rotation(point->box, rotation);
    }
}

SOREN_EXPORT float point_collider_scale(PointCollider* point) {
    return point->scale;
}

SOREN_EXPORT void point_collider_set_scale(PointCollider* point, float scale) {
    if (scale == point->scale) {
        return;
    }

    collider_assert_scale(scale);

    if (point->scale == 1 && scale != 1 && !point->box) {
        point->box = box_collider_create(1, 1);
        box_collider_set_original_center(point->box, vector_create(0.5, 0.5));
    }


    if (point->scale == 1) {
        box_collider_set_rotation(point->box, point->rotation);
        box_collider_set_position(point->box, point->position);
    }

    point->scale = scale;
    box_collider_set_scale(point->box, scale);
}

SOREN_EXPORT Vector point_collider_position(PointCollider* point) {
    return point->position;
}

SOREN_EXPORT void point_collider_set_position(PointCollider* point, Vector position) {
    if (vector_equals(position, point->position)) {
        return;
    }

    point->position = position;

    if(point->scale == 1) {
        box_collider_set_position(point->box, position);
    }
}

SOREN_EXPORT RectF point_collider_bounds(PointCollider* point) {
    if (point->scale == 1) {
        return (RectF){
            point->position.x,
            point->position.y,
            1,
            1
        };
    } else {
        return box_collider_bounds(point->box);
    }
}

SOREN_EXPORT void point_collider_debug_draw(PointCollider* point, SDL_Renderer* renderer, SDL_FColor color) {
    if (point->scale == 1) {
        draw_point_color(renderer, point->position, color);
    } else {
        box_collider_debug_draw(point->box, renderer, color);
    }
}

SOREN_EXPORT bool point_collider_overlaps_rect(PointCollider* collider, RectF rect) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_overlaps_rect(collider->box, rect);
    } else {
        return collision_point_to_rect(collider_position(collider), rect);
    }
}

SOREN_EXPORT bool point_collider_overlaps_collider(PointCollider* collider, Collider* other) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_overlaps_collider(collider->box, other);
    }

    Vector position = collider_position(collider);

    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return collision_point_to_box(position, point->box);
            }
            
            return vector_equals(position, collider_position(point));
        case COLLIDER_LINE:
            return collision_point_to_line(position, (LineCollider*)other);
        case COLLIDER_CIRCLE:
            return collision_point_to_circle(position, (CircleCollider*)other);
        case COLLIDER_BOX:
            return collision_point_to_box(position, (BoxCollider*)other);
        case COLLIDER_POLYGON:
            return collision_point_to_poly(position, (PolygonCollider*)other);
        default:
            throw(InvalidColliderType, "Invalid collider for point overlap check.");
            break;
    }

    return false;
}

SOREN_EXPORT bool point_collider_overlaps_line(PointCollider* collider, Vector start, Vector end) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_overlaps_line(collider->box, start, end);
    } else {
        return collision_point_to_segment(collider_position(collider), start, end);
    }
}

SOREN_EXPORT bool point_collider_contains_point(PointCollider* collider, Vector point) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_contains_point(collider->box, point);
    } else {
        return collision_point_to_point(collider_position(collider), point);
    }
}

SOREN_EXPORT bool point_collider_collides_rect(PointCollider* collider, RectF rect, CollisionResult* out_result) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_collides_rect(collider->box, rect, out_result);
    } else {
        return collision_point_to_rect_ext(collider_position(collider), rect, out_result);
    }
}

SOREN_EXPORT bool point_collider_collides_collider(PointCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_collides_collider(collider->box, other, out_result, out_hit);
    }

    Vector position = collider_position(collider);

    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return collision_point_to_box_ext(position, point->box, out_result);
            }
            
            return collision_point_to_point_ext(position, collider_position(point), out_result);
        case COLLIDER_LINE:
            return collision_point_to_line_ext(position, (LineCollider*)other, out_result);
        case COLLIDER_CIRCLE:
            return collision_point_to_circle_ext(position, (CircleCollider*)other, out_result);
        case COLLIDER_BOX:
            return collision_point_to_box_ext(position, (BoxCollider*)other, out_result);
        case COLLIDER_POLYGON:
            return collision_point_to_poly_ext(position, (PolygonCollider*)other, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider for point overlap check.");
            break;
    }

    return false;
}

SOREN_EXPORT bool point_collider_collides_line(PointCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_collides_line(collider->box, start, end, out_result);
    } else {
        CollisionResult collision_result = (CollisionResult){0};
        bool result = collision_point_to_segment_ext(collider_position(collider), start, end, &collision_result);
        if (result && out_result) {
            collision_result_to_raycast_hit(&collision_result, out_result);
        }

        return result;
    }
}

SOREN_EXPORT bool point_collider_collides_point(PointCollider* collider, Vector point, CollisionResult* out_result) {
    if (point_collider_using_internal_collider(collider)) {
        return box_collider_collides_point(collider->box, point, out_result);
    } else {
        return collision_point_to_point_ext(collider_position(collider), point, out_result);
    }
}