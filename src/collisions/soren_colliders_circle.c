#include <collisions/soren_colliders.h>

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