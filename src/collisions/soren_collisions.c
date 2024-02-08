#include <soren_std.h>
#include <collisions/soren_collisions.h>
#include <collisions/soren_collision_utils.h>

#include <float.h>
#include <math.h>

#define SOREN_POINT_RADIUS 1

static soren_thread_local BoxCollider* soren_shared_box_collider = NULL;

static BoxCollider* collision_shared_box_collider_init(RectF bounds) {
    if (!soren_shared_box_collider) {
        soren_shared_box_collider = box_collider_create(bounds.w, bounds.h);
        box_collider_set_position(soren_shared_box_collider, rectf_location(bounds));
    } else {
        box_collider_set_position(soren_shared_box_collider, rectf_location(bounds));
        box_collider_original_set_size(soren_shared_box_collider, rectf_size(bounds));
    }

    return soren_shared_box_collider;
}

void collision_result_remove_horizonal_translation(CollisionResult* result, Vector delta) {
    if (soren_sign(result->normal.x) != soren_sign(delta.x) 
        || (delta.x == 0 && result->normal.x != 0))
    {
        float response_distance = vector_length(result->minimum_translation_vector);
        float fix = response_distance / result->normal.y;

        if (abs(result->normal.x) != 1 && abs(fix) < abs(delta.y * 3)) {
            result->minimum_translation_vector = vector_create(0, -fix);
        }
    }
}

void collision_result_invert(CollisionResult* result) {
    result->minimum_translation_vector = vector_negate(result->minimum_translation_vector);
    result->normal = vector_negate(result->normal);
}

String* collision_result_to_string(CollisionResult* result, String* str) {
    if (!str) {
        str = string_create_ref("");
    }

    string_append(str, "CollisionResult { normal: ");
    vector_to_string(result->normal, str);
    string_append(str, ", mtv: ");
    vector_to_string(result->minimum_translation_vector, str);
    string_append(str, " }");

    return str;
}

void raycast_hit_reset(RaycastHit* hit) {
    hit->shape = NULL;
    hit->fraction = 0;
    hit->distance = 0;
}

String* raycast_hit_to_string(RaycastHit* hit, String* str) {
    str = string_format(str, "RaycastHit { fraction: %f, distance: %f, normal: ", hit->fraction, hit->distance);
    vector_to_string(hit->normal, str);
    string_append(str, ", centroid: ");
    vector_to_string(hit->centroid, str);
    string_append(str, ", point: ");
    vector_to_string(hit->point, str);
    string_append(str, " }");
    return str;
}

bool collision_circle_to_circle(CircleCollider* first, CircleCollider* second) {
    return collision_radius_to_radius(
        circle_collider_position(first),
        circle_collider_radius(first),
        circle_collider_position(second),
        circle_collider_radius(second));
}
bool collision_circle_to_circle_ext(CircleCollider* first, CircleCollider* second, CollisionResult* out_result) {
    return collision_radius_to_radius_ext(
        circle_collider_position(first),
        circle_collider_radius(first),
        circle_collider_position(second),
        circle_collider_radius(second),
        out_result);
}

bool collision_circle_to_radius(CircleCollider* circle, Vector position, float radius) {
    return collision_radius_to_radius(
        circle_collider_position(circle),
        circle_collider_radius(circle),
        position,
        radius);
}

bool collision_circle_to_radius_ext(CircleCollider* circle, Vector position, float radius, CollisionResult* out_result) {
    return collision_radius_to_radius_ext(
        circle_collider_position(circle),
        circle_collider_radius(circle),
        position,
        radius,
        out_result);
}

bool collision_radius_to_radius(Vector first_position, float first_radius, Vector second_position, float second_radius) {
    float distance = vector_distance_squared(first_position, second_position);
    float sum_of_radii = first_radius + second_radius;
    return distance < sum_of_radii * sum_of_radii;
}

bool collision_radius_to_radius_ext(Vector first_position, float first_radius, Vector second_position, float second_radius, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){ 0 };

    float distance = vector_distance_squared(first_position, second_position);
    float sum_of_radii = first_radius + second_radius;
    bool collided = distance < sum_of_radii * sum_of_radii;
    if (collided) {
        float depth = sum_of_radii - SDL_sqrtf(distance);

        result.normal = vector_normalize(vector_subtract(first_position, second_position));
        result.minimum_translation_vector = vector_multiply_scalar(result.normal, -depth);
        result.point = vector_add(second_position, vector_multiply_scalar(result.normal, second_radius));
    }

    if (out_result) {
        *out_result = result;
    }

    return collided;
}

bool collision_circle_to_box(CircleCollider* first, BoxCollider* second) {
    if (collider_rotation(second) != 0) {
        return collision_circle_to_polygon(first, (PolygonCollider*)second);
    }

    RectF bounds = box_collider_bounds(second);

    return collision_radius_to_rect(
        circle_collider_position(first),
        circle_collider_radius(first),
        &bounds
    );
}

bool collision_circle_to_box_ext(CircleCollider* first, BoxCollider* second, CollisionResult* out_result) {
    if (collider_rotation(second) != 0) {
        return collision_circle_to_polygon_ext(first, (PolygonCollider*)second, out_result);
    }

    RectF bounds = box_collider_bounds(second);

    return collision_radius_to_rect_ext(
        circle_collider_position(first),
        circle_collider_radius(first),
        &bounds,
        out_result
    );
}

bool collision_circle_to_rect(CircleCollider* first, RectF* second) {
    return collision_radius_to_rect(
        circle_collider_position(first),
        circle_collider_radius(first),
        second
    );
}

bool collision_circle_to_rect_ext(CircleCollider* first, RectF* second, CollisionResult* out_result) {
    return collision_radius_to_rect_ext(
        circle_collider_position(first),
        circle_collider_radius(first),
        second,
        out_result
    );
}

bool collision_radius_to_rect(Vector position, float radius, RectF* rect) {
    Vector point = closest_point_on_rectf_to_point(*rect, position);
    float distance = vector_distance_squared(point, position);

    if (distance <= radius * radius)
        return true;

    return false;
}

bool collision_radius_to_rect_ext(Vector position, float radius, RectF* rect, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    bool colliding = false;

    Vector point = closest_point_on_rectf_border_to_point(*rect, position, &result.normal);
    if (rectf_contains(*rect, position)) {
        result.point = point;

        Vector safe_point = vector_add(point, vector_multiply_scalar(result.normal, radius));
        result.minimum_translation_vector = vector_subtract(position, safe_point);
        colliding = true;
    } else {
        float distance = vector_distance_squared(point, position);
        if (distance == 0) {
            result.minimum_translation_vector = vector_multiply_scalar(result.normal, radius);
            colliding = true;
        } else if (distance <= radius * radius) {
            Vector offset = vector_subtract(position, point);
            float depth = vector_length(offset) - radius;

            result.normal = vector_normalize(offset);
            result.point = point;
            result.minimum_translation_vector = vector_multiply_scalar(result.normal, depth);

            colliding = true;
        }
    }

    if (out_result) {
        *out_result = result;
    }

    return colliding;
}

bool collision_circle_to_polygon(CircleCollider* first, PolygonCollider* second) {
    Vector position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));
    int count = 0;
    Vector* points = polygon_collider_points(second, &count);
    
    return collision_radius_to_shape(
        circle_collider_position(first),
        circle_collider_radius(first),
        points,
        count,
        position);
}

bool collision_circle_to_polygon_ext(CircleCollider* first, PolygonCollider* second, CollisionResult* out_result) {
    Vector position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));
    int count = 0;
    Vector* points = polygon_collider_points(second, &count);

    return collision_radius_to_shape_ext(
        circle_collider_position(first),
        circle_collider_radius(first),
        points,
        count,
        position,
        out_result);
}

bool collision_circle_to_shape(CircleCollider* first, Vector* points, int points_count, Vector shape_position) {
    return collision_radius_to_shape(
        circle_collider_position(first),
        circle_collider_radius(first),
        points,
        points_count,
        shape_position);
}

bool collision_circle_to_shape_ext(CircleCollider* first, Vector* points, int points_count, Vector shape_position, CollisionResult* out_result) {
    return collision_radius_to_shape_ext(
        circle_collider_position(first),
        circle_collider_radius(first),
        points,
        points_count,
        shape_position,
        out_result);
}

bool collision_radius_to_polygon(Vector position, float radius, PolygonCollider* second) {
    Vector poly_position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));
    int count = 0;
    Vector* points = polygon_collider_points(second, &count);

    return collision_radius_to_shape(
        position,
        radius,
        points,
        count,
        poly_position);
}

bool collision_radius_to_polygon_ext(Vector position, float radius, PolygonCollider* second, CollisionResult* out_result) {
    Vector poly_position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));
    int count = 0;
    Vector* points = polygon_collider_points(second, &count);

    return collision_radius_to_shape_ext(
        position,
        radius,
        points,
        count,
        poly_position,
        out_result);
}

bool collision_radius_to_shape(Vector position, float radius, Vector* points, int points_count, Vector shape_position) {
    Vector offset = vector_subtract(position, shape_position);

    if (collision_point_to_shape(offset, points, points_count, shape_position))
        return true;

    float distance_squared = 0;
    collisions_get_closest_point_on_polygon_to_point(points, points_count, offset, &distance_squared);

    return distance_squared <= radius * radius;
}

bool collision_radius_to_shape_ext(Vector position, float radius, Vector* points, int points_count, Vector shape_position, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    float distance = 0;

    Vector offset = vector_subtract(position, shape_position);
    Vector closest = collisions_get_closest_point_on_polygon_to_point_ext(
        points,
        points_count,
        offset,
        &distance,
        &result.normal
    );

    bool center_in_shape = collision_point_to_shape(offset, points, points_count, shape_position);
    if (distance > radius * radius && !center_in_shape)
        return false;

    if (center_in_shape) {
        result.minimum_translation_vector = vector_multiply_scalar(result.normal, SDL_sqrtf(distance) - radius);
    } else {
        if (distance == 0) {
            result.minimum_translation_vector = vector_multiply_scalar(result.normal, radius);
        } else {
            float true_distance = SDL_sqrtf(distance);
            Vector mtv = vector_negate(vector_subtract(offset, closest));
            mtv = vector_multiply_scalar(mtv, (radius - true_distance) / true_distance);
            result.minimum_translation_vector = mtv;
        }
    }

    result.point = vector_add(closest, shape_position);
    return true;
}

bool collision_polygon_to_polygon(PolygonCollider* first, PolygonCollider* second) {
    int first_count = 0;
    Vector* first_points = polygon_collider_points(first, &first_count);
    Vector* first_edges = polygon_collider_edge_normals(first, NULL);
    Vector first_position = vector_subtract(polygon_collider_position(first), polygon_collider_center(first));

    int second_count = 0;
    Vector* second_points = polygon_collider_points(second, &second_count);
    Vector* second_edges = polygon_collider_edge_normals(second, NULL);
    Vector second_position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));

    return collision_shape_to_shape(
        first_points,
        first_edges,
        first_count,
        first_position,
        second_points,
        second_edges,
        second_count,
        second_position);
}

bool collision_polygon_to_polygon_ext(PolygonCollider* first, PolygonCollider* second, CollisionResult* out_result) {
    int first_count = 0;
    Vector* first_points = polygon_collider_points(first, &first_count);
    Vector* first_edges = polygon_collider_edge_normals(first, NULL);
    Vector first_position = vector_subtract(polygon_collider_position(first), polygon_collider_center(first));

    int second_count = 0;
    Vector* second_points = polygon_collider_points(second, &second_count);
    Vector* second_edges = polygon_collider_edge_normals(second, NULL);
    Vector second_position = vector_subtract(polygon_collider_position(second), polygon_collider_center(second));

    return collision_shape_to_shape_ext(
        first_points,
        first_edges,
        first_count,
        first_position,
        second_points,
        second_edges,
        second_count,
        second_position,
        out_result);
}

bool collision_polygon_to_shape(PolygonCollider* first, Vector* points, Vector* edge_normals, int points_count, Vector shape_position) {
    int first_count = 0;
    Vector* first_points = polygon_collider_points(first, &first_count);
    Vector* first_edges = polygon_collider_edge_normals(first, NULL);
    Vector first_position = vector_subtract(polygon_collider_position(first), polygon_collider_center(first));

    return collision_shape_to_shape(
        first_points,
        first_edges,
        first_count,
        first_position,
        points,
        edge_normals,
        points_count,
        shape_position);
}

bool collision_polygon_to_shape_ext(PolygonCollider* first, Vector* points, Vector* edge_normals, int points_count, Vector shape_position, CollisionResult* out_result) {
    int first_count = 0;
    Vector* first_points = polygon_collider_points(first, &first_count);
    Vector* first_edges = polygon_collider_edge_normals(first, NULL);
    Vector first_position = vector_subtract(polygon_collider_position(first), polygon_collider_center(first));

    return collision_shape_to_shape_ext(
        first_points,
        first_edges,
        first_count,
        first_position,
        points,
        edge_normals,
        points_count,
        shape_position,
        out_result);
}

static inline void collision_shape_to_shape_get_interval(Vector axis, Vector* points, int points_count, float* out_min, float* out_max) {
    float dot = vector_dot(points[0], axis);
    *out_min = dot;
    *out_max = dot;

    for (int i = 1; i < points_count; i++) {
        dot = vector_dot(points[i], axis);
        if (dot < *out_min)
            *out_min = dot;
        
        if (dot > *out_max)
            *out_max = dot;
    }
}

static inline float collision_shape_to_shape_interval_distance(float min_a, float max_a, float min_b, float max_b) {
    if (min_a < min_b)
        return min_b - max_a;
    
    return min_a - max_b;
}

bool collision_shape_to_shape(
    Vector* first_points,
    Vector* first_edge_normals,
    int first_points_count,
    Vector first_position,
    Vector* second_points,
    Vector* second_edge_normals,
    int second_points_count,
    Vector second_position)
{
    bool intersecting = true;
    Vector offset = vector_subtract(first_position, second_position);
    Vector axis = VECTOR_ZERO;

    float min_a = 0;
    float max_a = 0;
    float min_b = 0;
    float max_b = 0;

    for (int edge_index = 0; edge_index < first_points_count + second_points_count; edge_index++) {
        if (edge_index < first_points_count) {
            axis = first_edge_normals[edge_index];
        } else {
            axis = second_edge_normals[edge_index - first_points_count];
        }

        collision_shape_to_shape_get_interval(axis, first_points, first_points_count, &min_a, &max_a);
        collision_shape_to_shape_get_interval(axis, second_points, second_points_count, &min_b, &max_b);

        float relative_interval_offset = vector_dot(offset, axis);
        min_a += relative_interval_offset;
        max_a += relative_interval_offset;

        float interval_distance = collision_shape_to_shape_interval_distance(min_a, max_a, min_b, max_b);
        if (interval_distance >= 0)
            return false;
    }

    return true;
}

bool collision_shape_to_shape_ext(
    Vector* first_points,
    Vector* first_edge_normals,
    int first_points_count,
    Vector first_position,
    Vector* second_points,
    Vector* second_edge_normals,
    int second_points_count,
    Vector second_position,
    CollisionResult* out_result)
{
    bool collides = false;
    CollisionResult result = (CollisionResult){0};
    bool intersecting = true;
    Vector offset = vector_subtract(first_position, second_position);
    Vector axis = VECTOR_ZERO;
    Vector translation_axis = VECTOR_ZERO;
    float min_interval_distance = FLT_MAX;

    float min_a = 0;
    float max_a = 0;
    float min_b = 0;
    float max_b = 0;

    for (int edge_index = 0; edge_index < first_points_count + second_points_count; edge_index++) {
        if (edge_index < first_points_count) {
            axis = first_edge_normals[edge_index];
        } else {
            axis = second_edge_normals[edge_index - first_points_count];
        }

        collision_shape_to_shape_get_interval(axis, first_points, first_points_count, &min_a, &max_a);
        collision_shape_to_shape_get_interval(axis, second_points, second_points_count, &min_b, &max_b);

        float relative_interval_offset = vector_dot(offset, axis);
        min_a += relative_interval_offset;
        max_a += relative_interval_offset;

        float interval_distance =  collision_shape_to_shape_interval_distance(min_a, max_a, min_b, max_b);
        if (interval_distance >= 0)
            goto end;

        if (interval_distance < 0)
            interval_distance *= -1;

        if (interval_distance < min_interval_distance) {

            min_interval_distance = interval_distance;
            translation_axis = axis;

            if (vector_dot(translation_axis, offset) < 0)
                translation_axis = vector_negate(translation_axis);
        }
    }

    collides = true;
    result.normal = translation_axis;
    result.minimum_translation_vector = vector_multiply_scalar(vector_negate(translation_axis), min_interval_distance);

    end:
        if (out_result) {
            *out_result = result;
        }

        return collides;
}

bool collision_box_to_box(BoxCollider* first, BoxCollider* second) {
    if (collider_rotation(first) != 0 || collider_rotation(second) != 0) {
        return collision_polygon_to_polygon((PolygonCollider*)first, (PolygonCollider*)second);
    }

    RectF first_bounds = collider_bounds(first);
    RectF second_bounds = collider_bounds(second);

    return collision_rect_to_rect(first_bounds, second_bounds);
}

bool collision_box_to_box_ext(BoxCollider* first, BoxCollider* second, CollisionResult* out_result) {
    if (collider_rotation(first) != 0 || collider_rotation(second) != 0) {
        return collision_polygon_to_polygon((PolygonCollider*)first, (PolygonCollider*)second);
    }

    RectF first_bounds = collider_bounds(first);
    RectF second_bounds = collider_bounds(second);

    return collision_rect_to_rect_ext(first_bounds, second_bounds, out_result);
}

bool collision_box_to_rect(BoxCollider* first, RectF second) {
    if (collider_rotation(first) != 0) {
        BoxCollider* shared = collision_shared_box_collider_init(second);
        return collision_polygon_to_polygon((PolygonCollider*)first, (PolygonCollider*)shared);
    }

    RectF first_bounds = collider_bounds(first);

    return collision_rect_to_rect(first_bounds, second);
}

bool collision_box_to_rect_ext(BoxCollider* first, RectF second, CollisionResult* out_result) {
    if (collider_rotation(first) != 0) {
        BoxCollider* shared = collision_shared_box_collider_init(second);
        return collision_polygon_to_polygon_ext((PolygonCollider*)first, (PolygonCollider*)shared, out_result);
    }

    RectF first_bounds = collider_bounds(first);

    return collision_rect_to_rect_ext(first_bounds, second, out_result);
}

bool collision_rect_to_rect(RectF first, RectF second) {
    return rectf_contains_or_intersects(first, second);
}

static inline RectF collision_rect_to_rect_minkowski_difference(RectF first, RectF second) {
    return (RectF) {
        first.x - rectf_right(second),
        first.y - rectf_bottom(second),
        first.w + second.w,
        first.h + second.h
    };
}

bool collision_rect_to_rect_ext(RectF first, RectF second, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    bool collided = false;
    RectF diff = collision_rect_to_rect_minkowski_difference(first, second);
    if (rectf_contains(diff, POINT_ZERO)) {
        result.minimum_translation_vector = rectf_closest_point_on_bounds_to_origin(diff);
        if (vector_equals(result.minimum_translation_vector, VECTOR_ZERO)) {
            goto end;
        }

        result.normal = vector_negate(result.minimum_translation_vector);
        result.normal = vector_normalize(result.normal);

        collided = true;
    }

    end:
        if (out_result) {
            *out_result = result;
        }

        return collided;
}

bool collision_point_to_circle(Vector point, CircleCollider* circle) {
    return collision_radius_to_radius(
        point,
        SOREN_POINT_RADIUS,
        circle_collider_position(circle),
        circle_collider_radius(circle));
}

bool collision_point_to_circle_ext(Vector point, CircleCollider* circle, CollisionResult* out_result) {
    return collision_radius_to_radius_ext(
        point,
        SOREN_POINT_RADIUS,
        circle_collider_position(circle),
        circle_collider_radius(circle),
        out_result);
}

bool collision_point_to_radius(Vector point, Vector circle_pos, float radius) {
    // Treat point as a circle with a radius of one.
    return collision_radius_to_radius(point, SOREN_POINT_RADIUS, circle_pos, radius);
}

bool collision_point_to_radius_ext(Vector point, Vector circle_pos, float radius, CollisionResult* out_result) {
    return collision_radius_to_radius(point, SOREN_POINT_RADIUS, circle_pos, radius, out_result);
}

bool collision_point_to_box(Vector point, BoxCollider* box) {
    if (collider_rotation(box) != 0) {
        return collision_point_to_poly(point, (PolygonCollider*)box);
    }

    RectF bounds = collider_bounds(box);
    return collision_point_to_rect(point, bounds);
}

bool collision_point_to_box(Vector point, BoxCollider* box, CollisionResult* out_result) {
    if (collider_rotation(box) != 0) {
        return collision_point_to_poly_ext(point, (PolygonCollider*)box, out_result);
    }

    RectF bounds = collider_bounds(box);
    return collision_point_to_rect_ext(point, bounds, out_result);
}

bool collision_point_to_rect(Vector point, RectF box) {
    return rectf_contains(box, point);
}

bool collision_point_to_rect_ext(Vector point, RectF box, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    bool collided = false;

    if (rectf_contains(box, point)) {
        result.point = closest_point_on_rectf_border_to_point(box, point, &result.normal);
        result.minimum_translation_vector = vector_subtract(point, result.point);
        collided = true;
    }

    if (out_result) {
        *out_result = result;
    }

    return collided;
}

bool collision_point_to_poly(Vector point, PolygonCollider* poly) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(poly), polygon_collider_center(poly));
    Vector* points = polygon_collider_points(poly, &points_count);

    return collision_point_to_shape(
        point,
        points,
        points_count,
        shape_position);
}

bool collision_point_to_poly_ext(Vector point, PolygonCollider* poly, CollisionResult* out_result) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(poly), polygon_collider_center(poly));
    Vector* points = polygon_collider_points(poly, &points_count);

    return collision_point_to_shape_ext(
        point,
        points,
        points_count,
        shape_position,
        out_result);
}

bool collision_point_to_shape(Vector point, Vector* points, int points_count, Vector shape_position) {
    point = vector_subtract(point, shape_position);
    bool inside = false;

    // Unreadable implementation of the crossing number algorithm (also known as the ray casting algorithm)
    // that can be used to determine if a point is inside of a polygon.
    // This specific implementation is taken from Nez
    // (https://github.com/prime31/Nez/blob/2eced59116030af9a176ca5b4cd3e9bdf882ad07/Nez.Portable/Physics/Shapes/Polygon.cs#L351)
    // From their documentation:
    // Essentially what the algorithm is doing is shooting a ray from point out. If it intersects an odd number of polygon sides
    // we know it is inside the polygon.
    for (int i = 0, j = points_count - 1; i < points_count; j = i++) {
        if (((points[i].y > point.y) != (points[j].y > point.y))
            && (point.x < (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
        {
            inside = !inside;
        }
    }

    return inside;
}

bool collision_point_to_shape_ext(Vector point, Vector* points, int points_count, Vector shape_position, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};

    bool collides = collision_point_to_shape(point, points, points_count, shape_position);
    if (collides) {
        float distance_squared = 0;
        Vector closest = collisions_get_closest_point_on_polygon_to_point_ext(points, points_count, point, &distance_squared, &result.normal);

        result.minimum_translation_vector = vector_multiply_scalar(result.normal, sqrtf(distance_squared));
        result.point = vector_add(closest, shape_position);
    }

    if (out_result) {
        *out_result = result;
    }

    return collides;
}

bool collision_point_to_line(Vector point, LineCollider* line) {
    return collision_point_to_segment(
        point,
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line));
}

bool collision_point_to_line_ext(Vector point, LineCollider* line, CollisionResult* out_result) {
    return collision_point_to_segment_ext(
        point,
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        out_result);
}

bool collision_point_to_segment(Vector point, Vector line_start, Vector line_end) {
    float distances = vector_distance(line_start, point) + vector_distance(point, line_end);
    return distances == vector_distance(line_start, line_end);
}

bool collision_point_to_segment_ext(Vector point, Vector line_start, Vector line_end, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    bool collided = false;
    if (collision_point_to_segment(point, line_start, line_end)) {
        Vector perp = vector_perpendicular(line_start, line_end);
        perp = vector_normalize(perp);

        result.minimum_translation_vector = perp;
        result.normal = perp;
        result.point = point;
        collided = true;
    }

    if (out_result) {
        *out_result = result;
    }

    return collided;
}

bool collision_point_to_point(Vector first, Vector second) {
    return vector_equals(first, second);
}
bool collision_point_to_point_ext(Vector first, Vector second, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    bool collides = vector_equals(first, second);
    if (collides) {
        result.minimum_translation_vector = vector_create(0, -1);
        result.normal = vector_create(0, -1);
        result.point = first;
    }

    if (out_result) {
        *out_result = result;
    }

    return collides;
}

bool collision_line_to_poly(LineCollider* line, PolygonCollider* polygon) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(polygon), polygon_collider_center(polygon));
    Vector* points = polygon_collider_points(polygon, &points_count);

    return collision_segment_to_shape(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        points,
        points_count,
        shape_position);
}

bool collision_line_to_poly(LineCollider* line, PolygonCollider* polygon, RaycastHit* out_result) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(polygon), polygon_collider_center(polygon));
    Vector* points = polygon_collider_points(polygon, &points_count);

    return collision_segment_to_shape_ext(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        points,
        points_count,
        shape_position,
        out_result);
}

bool collision_segment_to_poly(Vector start, Vector end, PolygonCollider* polygon) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(polygon), polygon_collider_center(polygon));
    Vector* points = polygon_collider_points(polygon, &points_count);

    return collision_segment_to_shape(
        start,
        end,
        points,
        points_count,
        shape_position);
}

bool collision_segment_to_poly_ext(Vector start, Vector end, PolygonCollider* polygon, RaycastHit* out_result) {
    int points_count = 0;
    Vector shape_position = vector_subtract(polygon_collider_position(polygon), polygon_collider_center(polygon));
    Vector* points = polygon_collider_points(polygon, &points_count);
    
    return collision_segment_to_shape_ext(
        start,
        end,
        points,
        points_count,
        shape_position,
        out_result);
}

bool collision_segment_shape(Vector start, Vector end, Vector* points, int points_count, Vector shape_position) {
    for (int i = 0, j = points_count - 1; i < points_count; j = i++) {
        Vector edge_start = vector_add(points[j], shape_position);
        Vector edge_end = vector_add(points[i], shape_position);

        if (collision_segment_to_segment(start, end, edge_start, edge_end)) {
            return true;
        }
    }

    return false;
}

bool collision_segment_shape_ext(Vector start, Vector end, Vector* points, int points_count, Vector shape_position, RaycastHit* out_result) {
    RaycastHit hit = (RaycastHit){0};
    Vector normal = VECTOR_ZERO;
    Vector intersection_point = VECTOR_ZERO;
    float fraction = FLT_MAX;
    bool intersects = false;

    for (int i = 0, j = points_count - 1; i < points_count; j = i++) {
        Vector edge_start = vector_add(points[j], shape_position);
        Vector edge_end = vector_add(points[i], shape_position);

        Vector intersection;
        if (collision_segment_to_segment_intersection(start, end, edge_start, edge_end, &intersection)) {
            intersects = true;

            float distance_fraction = (intersection.x - start.x) / (end.x - start.x);
            if (isnan(distance_fraction)) {
                distance_fraction = (intersection.y - start.y) / (end.y - start.y);
            }

            if (distance_fraction < fraction) {
                Vector edge = vector_subtract(edge_start, edge_end);
                normal.x = edge.y;
                normal.y = -edge.x;
                fraction = distance_fraction;
                intersection_point = intersection_point;
            }
        }
    }

    if (intersects) {
        hit.fraction = fraction;
        hit.distance = vector_distance(start, intersection_point);
        hit.point = intersection_point;
        hit.normal = vector_normalize(normal);
    }

    if (out_result) {
        *out_result = hit;
    }

    return intersects;
}

bool collision_line_to_circle(LineCollider* line, CircleCollider* circle) {
    return collision_segment_to_radius(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        circle_collider_position(circle),
        circle_collider_radius(circle));
}

bool collision_line_to_circle_ext(LineCollider* line, CircleCollider* circle, RaycastHit* out_result) {
    return collision_segment_to_radius_ext(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        circle_collider_position(circle),
        circle_collider_radius(circle),
        out_result);
}

bool collision_segment_to_circle(Vector start, Vector end, CircleCollider* circle) {
    return collision_segment_to_radius(
        start,
        end,
        circle_collider_position(circle),
        circle_collider_radius(circle));
}

bool collision_segment_to_circle_ext(Vector start, Vector end, CircleCollider* circle, RaycastHit* out_result) {
    return collision_segment_to_radius_ext(
        start,
        end,
        circle_collider_position(circle),
        circle_collider_radius(circle),
        out_result);
}

bool collision_line_to_radius(LineCollider* line, Vector position, float radius) {
    return collision_segment_to_radius(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        position,
        radius);
}

bool collision_line_to_radius_ext(LineCollider* line, Vector position, float radius, RaycastHit* out_result) {
    return collision_segment_to_radius_ext(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        position,
        radius,
        out_result);
}

bool collision_segment_to_radius(Vector start, Vector end, Vector position, float radius) {
    float line_length = vector_distance(start, end);
    Vector d = vector_divide_scalar(vector_subtract(end, start), line_length);
    Vector m = vector_subtract(start, position);
    float b = vector_dot(m, d);
    float c = vector_dot(m, m) - radius * radius;

    if (c > 0 && b > 0) {
        return false;
    }

    float discriminant = b * b - c;

    if (discriminant < 0) {
        return false;
    }

    return true;
}

bool collision_segment_to_radius_ext(Vector start, Vector end, Vector position, float radius, RaycastHit* out_result) {
    RaycastHit hit = (RaycastHit){0};
    bool collides = false;

    float line_length = vector_distance(start, end);
    Vector d = vector_divide_scalar(vector_subtract(end, start), line_length);
    Vector m = vector_subtract(start, position);
    float b = vector_dot(m, d);
    float c = vector_dot(m, m) - radius * radius;

    if (c > 0 && b > 0) {
        goto end;
    }

    float discriminant = b * b - c;

    if (discriminant < 0) {
        goto end;
    }

    hit.fraction = -b - sqrtf(discriminant);
    if (hit.fraction < 0) {
        hit.fraction = 0;
    }

    hit.point = vector_add(start, vector_multiply_scalar(d, hit.fraction));
    hit.distance = vector_distance(start, hit.point);
    hit.normal = vector_normalize(vector_subtract(hit.point, position));
    hit.fraction = hit.distance / line_length;

    collides = true;

    end:
        if (out_result) {
            *out_result = hit;
        }

        return collides;
}

bool collision_line_to_line(LineCollider* first, LineCollider* second) {
    return collision_segment_to_segment_intersection(
        line_collider_adjusted_start(first),
        line_collider_adjusted_end(first),
        line_collider_adjusted_start(second),
        line_collider_adjusted_end(second),
        NULL);
}

bool collision_line_to_line_ext(LineCollider* first, LineCollider* second, CollisionResult* out_result) {
    return collision_segment_to_segment_ext(
        line_collider_adjusted_start(first),
        line_collider_adjusted_end(first),
        line_collider_adjusted_start(second),
        line_collider_adjusted_end(second),
        out_result);
}

bool collision_line_to_segment(LineCollider* line, Vector start, Vector end) {
    return collision_segment_to_segment_intersection(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        start,
        end,
        NULL);
}

bool collision_line_to_segment_ext(LineCollider* line, Vector start, Vector end, CollisionResult* out_result) {
    return collision_segment_to_segment_ext(
        line_collider_adjusted_start(line),
        line_collider_adjusted_end(line),
        start,
        end,
        out_result);
}

bool collision_segment_to_segment_intersection(Vector first_start, Vector first_end, Vector second_start, Vector second_end, Vector* out_intersection) {
    Vector intersection = VECTOR_ZERO;
    
    Vector b = vector_subtract(first_end, first_start);
    Vector d = vector_subtract(second_end, first_end);
    float b_dot_d_perp = b.x * d.y - b.y * d.x;
    bool collides = false;

    // if b dot d == 0, it means the lines are parallel
    if (b_dot_d_perp == 0) {
        goto end;
    }

    Vector c = vector_subtract(second_start, first_start);
    float t = (c.x * d.y - c.y * d.x) / b_dot_d_perp;
    if (t < 0 || t > 1) {
        goto end;
    }

    float u = (c.x * b.y - c.y * b.x) / b_dot_d_perp;
    if (u < 0 || u > 1) {
        goto end;
    }

    intersection = vector_add(first_start, vector_multiply_scalar(b, t));
    collides = true;

    end:
        if (out_intersection) {
            *out_intersection = intersection;
        }

        return collides;
}

bool collision_segment_to_segment(Vector first_start, Vector first_end, Vector second_start, Vector second_end) {
    return collision_segment_to_segment_intersection(
        first_start,
        first_end,
        second_start,
        second_end,
        NULL);
}

bool collision_segment_to_segment_ext(Vector first_start, Vector first_end, Vector second_start, Vector second_end, CollisionResult* out_result) {
    CollisionResult result = (CollisionResult){0};
    Vector intersection;

    bool collides = collision_segment_to_segment_intersection(
        first_start,
        first_end,
        second_start,
        second_end,
        &intersection);

    if (!collides) {
        goto end;
    }

    Vector min_vector = first_start;
    float min_distance = vector_distance_squared(intersection, first_start);
    
    float next_distance = vector_distance_squared(intersection, first_end);
    if (next_distance < min_distance) {
        min_distance = next_distance;
        min_vector = first_end;
    }

    next_distance = vector_distance_squared(intersection, second_start);
    if (next_distance < min_distance) {
        min_distance = next_distance;
        min_vector = second_start;
    }

    

    next_distance = vector_distance_squared(intersection, second_end);
    if (next_distance < min_distance) {
        min_distance = next_distance;
        min_vector = second_end;
    }

    result.minimum_translation_vector = vector_subtract(min_vector, intersection);
    result.point = intersection;

    // Todo: Don't think this normal is right.
    result.normal = vector_normalize(result.minimum_translation_vector);

    collides = true;

    end:
        if (out_result) {
            *out_result = result;
        }

        return collides;
}