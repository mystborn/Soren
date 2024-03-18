#include <collisions\soren_collision_utils.h>

#include <float.h>

SOREN_EXPORT void collisions_build_box(Vector* points, float width, float height) {
    points[0] = vector_create(0, 0);
    points[1] = vector_create(width, 0);
    points[2] = vector_create(width, height);
    points[3] = vector_create(0, height);
}

SOREN_EXPORT Vector collisions_polygon_find_center(Vector* points, int count) {
    float x = 0;
    float y = 0;

    for (int i = 0; i < count; i++) {
        x += points[i].x;
        y += points[i].y;
    }

    return vector_create(x / count, y / count);
}

SOREN_EXPORT Vector collisions_get_farthest_point_in_direction(
    Vector* points, 
    int count, 
    Vector direction)
{
    int index = 0;
    float max_dot = vector_dot(points[index], direction);

    for (int i = 1; i < count; i++) {
        float dot = vector_dot(points[i], direction);
        if (dot > max_dot) {
            max_dot = dot;
            index = i;
        }
    }

    return points[index];
}

SOREN_EXPORT Vector collisions_get_closest_point_on_polygon_to_point(
    Vector* points,
    int count,
    Vector point,
    float* out_distance_squared)
{
    float distance_squared = FLT_MAX;
    Vector closest_point = VECTOR_ZERO;

    for (int i = 0; i < count; i++) {
        int j = i + 1;
        if (j == count) {
            j = 0;
        }

        Vector closest = collisions_closest_point_on_line(points[i], points[j], point);
        float temp_distance_squared = vector_distance_squared(point, closest);

        if (temp_distance_squared < distance_squared) {
            distance_squared = temp_distance_squared;
            closest_point = closest;
        }
    }

    if (out_distance_squared) {
        *out_distance_squared = distance_squared;
    }

    return closest_point;
}

SOREN_EXPORT Vector collisions_get_closest_point_on_polygon_to_point_ext(
    Vector* points,
    int count,
    Vector point,
    float* out_distance_squared,
    Vector* out_edge_normal)
{
    float distance_squared = FLT_MAX;
    Vector closest_point = VECTOR_ZERO;
    Vector edge_normal = VECTOR_ZERO;

    for (int i = 0; i < count; i++) {
        int j = i + 1;
        if (j == count) {
            j = 0;
        }

        Vector closest = collisions_closest_point_on_line(points[i], points[j], point);
        float temp_distance_squared = vector_distance_squared(point, closest);

        if (temp_distance_squared < distance_squared) {
            distance_squared = temp_distance_squared;
            closest_point = closest;

            Vector line = vector_subtract(points[j], points[i]);
            edge_normal.x = -line.y;
            edge_normal.y = line.x;
        }
    }

    if (out_distance_squared) {
        *out_distance_squared = distance_squared;
    }

    if (out_edge_normal) {
        *out_edge_normal = vector_normalize(edge_normal);
    }

    return closest_point;
}

SOREN_EXPORT PolygonCollider* polygon_collider_create_convex_from_points(
    Vector* points,
    int count)
{
    throw(NotImplementedException, "polygon_collider_create_convex_from_points not implemented");
    return NULL;
}

SOREN_EXPORT Vector collisions_closest_point_on_line(Vector start, Vector end, Vector closest) {
    Vector v = vector_subtract(end, start);
    Vector w = vector_subtract(closest, start);
    float t = vector_dot(w, v) / vector_dot(v, v);
    t = SDL_clamp(t, 0, 1);

    return vector_add(start, vector_multiply_scalar(v, t));
}