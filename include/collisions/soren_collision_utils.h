#ifndef SOREN_COLLISIONS_SOREN_COLLISION_UTILS_H
#define SOREN_COLLISIONS_SOREN_COLLISION_UTILS_H

#include "../soren_math.h"
#include "soren_colliders.h"

void collisions_build_box(Vector* points, float width, float height);

Vector collisions_polygon_find_center(Vector* points, int count);

Vector collisions_get_farthest_point_in_direction(
    Vector* points, 
    int count, 
    Vector direction);

Vector collisions_get_closest_point_on_polygon_to_point(
    Vector* points,
    int count,
    Vector point,
    float* out_distance_squared);

Vector collisions_get_closest_point_on_polygon_to_point_ext(
    Vector* points,
    int count,
    Vector point,
    float* out_distance_squared,
    Vector* out_edge_normal);

PolygonCollider* polygon_collider_create_convex_from_points(
    Vector* points,
    int count);

Vector collisions_closest_point_on_line(Vector start, Vector end, Vector closest);

#endif