#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>

CircleCollider* circle_collider_create(float radius) {
    CircleCollider* collider = soren_malloc(sizeof(*collider));
    if (!collider)
        return NULL;

    circle_collider_init(collider, radius);
    return collider;
}

void circle_collider_init(CircleCollider* circle, float radius) {
    E4C_ASSERT(circle);
    collider_init((Collider*)circle, COLLIDER_CIRCLE);
    circle->radius = radius;
    circle->position = (Vector){0};
    circle->scale = 1;
}

float circle_collider_rotation(CircleCollider* circle) {
    return 0;
}

void circle_collider_set_rotation(CircleCollider* circle, float rotation) {
}

float circle_collider_scale(CircleCollider* circle) {
    return circle->scale;
}

void circle_collider_set_scale(CircleCollider* circle, float scale) {
    circle->scale = scale;
}

Vector circle_collider_position(CircleCollider* circle) {
    return circle->position;
}

void circle_collider_set_position(CircleCollider* circle, Vector position) {
    circle->position = position;
}

RectF circle_collider_bounds(CircleCollider* circle) {
    float radius = circle_collider_radius(circle);
    return (RectF){ 
        circle->position.x - radius,
        circle->position.y - radius,
        radius * 2,
        radius * 2
    };
}

float circle_collider_radius(CircleCollider* circle) {
    return circle->radius * circle->scale;
}

float circle_collider_original_radius(CircleCollider* circle) {
    return circle->radius;
}

void circle_collider_set_original_radius(CircleCollider* circle, float value) {
    circle->radius = value;
}

bool circle_collider_overlaps_rect(CircleCollider* collider, RectF rect) {
    return collision_circle_to_rect(collider, rect);
}

bool circle_collider_overlaps_collider(CircleCollider* collider, Collider* other) {
    // switch (other->collider_type) {
    //     case COLLIDER_POINT:
    //     case COLLIDER_LINE:
    //     case COLLIDER_CIRCLE:
    //     case COLLIDER_BOX:
    //     case COLLIDER_POLYGON:
    //     default:
    //         throw(InvalidColliderType, "Invalid collider for circle overlap check.");
    //         break;
    // }
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return circle_collider_overlaps_collider(collider, (Collider*)point->box);
            } else {
                return circle_collider_contains_point(collider, point_collider_position(point));
            }
        case COLLIDER_LINE:
            return collision_line_to_circle((LineCollider*)other, collider);
        case COLLIDER_CIRCLE:
            return collision_circle_to_circle(collider, (CircleCollider*)other);
        case COLLIDER_BOX:
            BoxCollider* box = (BoxCollider*)other;
            if (box_collider_rotation(box) == 0) {
                return collision_circle_to_box(collider, box);
            } else {
                return collision_circle_to_polygon(collider, (PolygonCollider*)box);
            }
        case COLLIDER_POLYGON:
            return collision_circle_to_polygon(collider, (PolygonCollider*)other);
        default:
            throw(InvalidColliderType, "Invalid collider for circle overlap check.");
            break;
    }

    return false;
}

bool circle_collider_overlaps_line(CircleCollider* collider, Vector start, Vector end) {
    return collision_segment_to_circle(start, end, collider);
}

bool circle_collider_contains_point(CircleCollider* collider, Vector point) {
    return collision_point_to_circle(point, collider);
}

bool circle_collider_collides_rect(CircleCollider* collider, RectF rect, CollisionResult* out_result) {
    return collision_circle_to_rect_ext(collider, rect, out_result);
}

bool circle_collider_collides_collider(CircleCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return circle_collider_collides_collider(collider, (Collider*)point->box, out_result, out_hit);
            } else {
                return circle_collider_collides_point(collider, point_collider_position(point), out_result);
            }
        case COLLIDER_LINE:
            return collision_line_to_circle_ext((LineCollider*)other, collider, out_hit);
        case COLLIDER_CIRCLE:
            return collision_circle_to_circle_ext(collider, (CircleCollider*)other, out_result);
        case COLLIDER_BOX:
            BoxCollider* box = (BoxCollider*)other;
            if (box_collider_rotation(box) == 0) {
                return collision_circle_to_box_ext(collider, box, out_result);
            } else {
                return collision_circle_to_polygon_ext(collider, (PolygonCollider*)box, out_result);
            }
        case COLLIDER_POLYGON:
            return collision_circle_to_polygon_ext(collider, (PolygonCollider*)other, out_result);
        default:
            throw(InvalidColliderType, "Invalid collider for circle collides check.");
            break;
    }

    return false;
}

bool circle_collider_collides_line(CircleCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    return collision_segment_to_circle_ext(start, end, collider, out_result);
}

bool circle_collider_collides_point(CircleCollider* collider, Vector point, CollisionResult* out_result) {
    return collision_point_to_circle_ext(point, collider, out_result);
}