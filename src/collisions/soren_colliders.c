#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>

#include "soren_collisions_shared.h"

E4C_DEFINE_EXCEPTION(InvalidColliderType, "Invalid Collider Type", RuntimeException);

void collider_assert_scale(float scale) {
    if (scale < SDL_FLT_EPSILON) {
        throw(IllegalArgumentException, "Scale cannot be less than or equal to zero");
    }
}

void collider_init(Collider* collider, ColliderType type) {
    collider->collider_type = type;
    collider->tag = NULL;
    collider->id = 0;
}

void collider_free(Collider* collider) {
    collider_free_resources(collider);

    SDL_free(collider);
}

void collider_free_resources(Collider* collider) {
    switch (collider->collider_type) {
        case COLLIDER_POLYGON:
            polygon_collider_free_resources((PolygonCollider*)collider);
            break;
        case COLLIDER_BOX:
            polygon_collider_free_resources((PolygonCollider*)collider);
            break;
        case COLLIDER_POINT:
            point_collider_free_resources((PointCollider*)collider);
            break;
        default:
            break;
    }
}

float collider_rotation_impl(Collider* collider) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_rotation((PointCollider*)collider);
        case COLLIDER_LINE:
            return line_collider_rotation((LineCollider*)collider);
        case COLLIDER_CIRCLE:
            return circle_collider_rotation((CircleCollider*)collider);
        case COLLIDER_BOX:
            return box_collider_rotation((BoxCollider*)collider);
        case COLLIDER_POLYGON:
            return polygon_collider_rotation((PolygonCollider*)collider);
        default:
            return 0;
    }
}

void collider_set_rotation_impl(Collider* collider, float rotation) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            point_collider_set_rotation((PointCollider*)collider, rotation);
            break;
        case COLLIDER_LINE:
            line_collider_set_rotation((LineCollider*)collider, rotation);
            break;
        case COLLIDER_CIRCLE:
            circle_collider_set_rotation((CircleCollider*)collider, rotation);
            break;
        case COLLIDER_BOX:
            box_collider_set_rotation((BoxCollider*)collider, rotation);
            break;
        case COLLIDER_POLYGON:
            polygon_collider_set_rotation((PolygonCollider*)collider, rotation);
            break;
        default:
            break;
    }
}

float collider_scale_impl(Collider* collider) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_scale((PointCollider*)collider);
        case COLLIDER_LINE:
            return line_collider_scale((LineCollider*)collider);
        case COLLIDER_CIRCLE:
            return circle_collider_scale((CircleCollider*)collider);
        case COLLIDER_BOX:
            return box_collider_scale((BoxCollider*)collider);
        case COLLIDER_POLYGON:
            return polygon_collider_scale((PolygonCollider*)collider);
        default:
            return 0;
    }
}

void collider_set_scale_impl(Collider* collider, float scale) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            point_collider_set_scale((PointCollider*)collider, scale);
            break;
        case COLLIDER_LINE:
            line_collider_set_scale((LineCollider*)collider, scale);
            break;
        case COLLIDER_CIRCLE:
            circle_collider_set_scale((CircleCollider*)collider, scale);
            break;
        case COLLIDER_BOX:
            box_collider_set_scale((BoxCollider*)collider, scale);
            break;
        case COLLIDER_POLYGON:
            polygon_collider_set_scale((PolygonCollider*)collider, scale);
            break;
        default:
            break;
    }
}

Vector collider_position_impl(Collider* collider) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_position((PointCollider*)collider);
        case COLLIDER_LINE:
            return line_collider_position((LineCollider*)collider);
        case COLLIDER_CIRCLE:
            return circle_collider_position((CircleCollider*)collider);
        case COLLIDER_BOX:
            return box_collider_position((BoxCollider*)collider);
        case COLLIDER_POLYGON:
            return polygon_collider_position((PolygonCollider*)collider);
        default:
            return (Vector){0, 0};
    }
}

void collider_set_position_impl(Collider* collider, Vector position) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            point_collider_set_position((PointCollider*)collider, position);
            break;
        case COLLIDER_LINE:
            line_collider_set_position((LineCollider*)collider, position);
            break;
        case COLLIDER_CIRCLE:
            circle_collider_set_position((CircleCollider*)collider, position);
            break;
        case COLLIDER_BOX:
            box_collider_set_position((BoxCollider*)collider, position);
            break;
        case COLLIDER_POLYGON:
            polygon_collider_set_position((PolygonCollider*)collider, position);
            break;
        default:
            break;
    }
}

RectF collider_bounds_impl(Collider* collider) {
    switch(collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_bounds((PointCollider*)collider);
        case COLLIDER_LINE:
            return line_collider_bounds((LineCollider*)collider);
        case COLLIDER_CIRCLE:
            return circle_collider_bounds((CircleCollider*)collider);
        case COLLIDER_BOX:
            return box_collider_bounds((BoxCollider*)collider);
        case COLLIDER_POLYGON:
            return polygon_collider_bounds((PolygonCollider*)collider);
        default:
            return (RectF){0, 0, 0, 0};
    }
}

bool collider_overlaps_rect_impl(Collider* collider, RectF rect) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_overlaps_rect((PointCollider*)collider, rect);
        case COLLIDER_LINE:
            return line_collider_overlaps_rect((LineCollider*)collider, rect);
        case COLLIDER_CIRCLE:
            return circle_collider_overlaps_rect((CircleCollider*)collider, rect);
        case COLLIDER_BOX:
            return box_collider_overlaps_rect((BoxCollider*)collider, rect);
        case COLLIDER_POLYGON:
            return polygon_collider_overlaps_rect((PolygonCollider*)collider, rect);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_overlaps_collider_impl(Collider* collider, Collider* other) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_overlaps_collider((PointCollider*)collider, other);
        case COLLIDER_LINE:
            return line_collider_overlaps_collider((LineCollider*)collider, other);
        case COLLIDER_CIRCLE:
            return circle_collider_overlaps_collider((CircleCollider*)collider, other);
        case COLLIDER_BOX:
            return box_collider_overlaps_collider((BoxCollider*)collider, other);
        case COLLIDER_POLYGON:
            return polygon_collider_overlaps_collider((PolygonCollider*)collider, other);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_overlaps_line_impl(Collider* collider, Vector start, Vector end) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_overlaps_line((PointCollider*)collider, start, end);
        case COLLIDER_LINE:
            return line_collider_overlaps_line((LineCollider*)collider, start, end);
        case COLLIDER_CIRCLE:
            return circle_collider_overlaps_line((CircleCollider*)collider, start, end);
        case COLLIDER_BOX:
            return box_collider_overlaps_line((BoxCollider*)collider, start, end);
        case COLLIDER_POLYGON:
            return polygon_collider_overlaps_line((PolygonCollider*)collider, start, end);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_contains_point_impl(Collider* collider, Vector point) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_contains_point((PointCollider*)collider, point);
        case COLLIDER_LINE:
            return line_collider_contains_point((LineCollider*)collider, point);
        case COLLIDER_CIRCLE:
            return circle_collider_contains_point((CircleCollider*)collider, point);
        case COLLIDER_BOX:
            return box_collider_contains_point((BoxCollider*)collider, point);
        case COLLIDER_POLYGON:
            return polygon_collider_contains_point((PolygonCollider*)collider, point);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_collides_rect_impl(Collider* collider, RectF rect, CollisionResult* out_result) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_collides_rect((PointCollider*)collider, rect, out_result);
        case COLLIDER_LINE:
            return line_collider_collides_rect((LineCollider*)collider, rect, out_result);
        case COLLIDER_CIRCLE:
            return circle_collider_collides_rect((CircleCollider*)collider, rect, out_result);
        case COLLIDER_BOX:
            return box_collider_collides_rect((BoxCollider*)collider, rect, out_result);
        case COLLIDER_POLYGON:
            return polygon_collider_collides_rect((PolygonCollider*)collider, rect, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_collides_collider_impl(Collider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_collides_collider((PointCollider*)collider, other, out_result, out_hit);
        case COLLIDER_LINE:
            return line_collider_collides_collider((LineCollider*)collider, other, out_result, out_hit);
        case COLLIDER_CIRCLE:
            return circle_collider_collides_collider((CircleCollider*)collider, other, out_result, out_hit);
        case COLLIDER_BOX:
            return box_collider_collides_collider((BoxCollider*)collider, other, out_result, out_hit);
        case COLLIDER_POLYGON:
            return polygon_collider_collides_collider((PolygonCollider*)collider, other, out_result, out_hit);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_collides_line_impl(Collider* collider, Vector start, Vector end, RaycastHit* out_result) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_collides_line((PointCollider*)collider, start, end, out_result);
        case COLLIDER_LINE:
            return line_collider_collides_line((LineCollider*)collider, start, end, out_result);
        case COLLIDER_CIRCLE:
            return circle_collider_collides_line((CircleCollider*)collider, start, end, out_result);
        case COLLIDER_BOX:
            return box_collider_collides_line((BoxCollider*)collider, start, end, out_result);
        case COLLIDER_POLYGON:
            return polygon_collider_collides_line((PolygonCollider*)collider, start, end, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}

bool collider_collides_point_impl(Collider* collider, Vector point, CollisionResult* out_result) {
    switch (collider->collider_type) {
        case COLLIDER_POINT:
            return point_collider_collides_point((PointCollider*)collider, point, out_result);
        case COLLIDER_LINE:
            return line_collider_collides_point((LineCollider*)collider, point, out_result);
        case COLLIDER_CIRCLE:
            return circle_collider_collides_point((CircleCollider*)collider, point, out_result);
        case COLLIDER_BOX:
            return box_collider_collides_point((BoxCollider*)collider, point, out_result);
        case COLLIDER_POLYGON:
            return polygon_collider_collides_point((PolygonCollider*)collider, point, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider type");
            break;
    }

    return false;
}
