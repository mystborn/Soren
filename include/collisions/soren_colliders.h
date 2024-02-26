#ifndef SOREN_COLLISIONS_SOREN_COLLIDERS_H
#define SOREN_COLLISIONS_SOREN_COLLIDERS_H

#include <stdbool.h>

#include <SDL3/SDL.h>

#include "../soren_std.h"
#include "../soren_math.h"

E4C_DECLARE_EXCEPTION(InvalidColliderType);

typedef enum ColliderType {
    COLLIDER_POINT,
    COLLIDER_LINE,
    COLLIDER_CIRCLE,
    COLLIDER_BOX,
    COLLIDER_POLYGON,
    // COLLIDER_CAPSULE
} ColliderType;

typedef struct Collider {
    ColliderType collider_type;
    void* tag;
    int id;
} Collider;

typedef struct CircleCollider {
    Collider base;
    Vector position;
    float radius;
    float scale;
} CircleCollider;

typedef struct PolygonCollider {
    Collider base;
    RectF bounding_box;
    Vector* edge_normals;
    Vector* points;
    Vector* original_points;
    Vector position;
    Vector original_center;
    float scale;
    float rotation;
    int points_count;
    bool dirty;
} PolygonCollider;

typedef struct BoxCollider {
    PolygonCollider base;
    Vector size;
} BoxCollider;

typedef struct PointCollider {
    Collider base;
    BoxCollider* box;
    Vector position;
    float rotation;
    float scale;
} PointCollider;

typedef struct LineCollider {
    Collider base;
    RectF bounding_box;
    Vector position;
    Vector start;
    Vector end;
    Vector pivot;
    Vector original_start;
    Vector original_end;
    Vector original_pivot;
    float rotation;
    float scale;
    bool dirty;
} LineCollider;

typedef struct CollisionResult CollisionResult;
typedef struct RaycastHit RaycastHit;

SOREN_EXPORT void collider_init(Collider* collider, ColliderType type);
SOREN_EXPORT void collider_free_resources(Collider* collider);
SOREN_EXPORT void collider_free(Collider* collider);

SOREN_EXPORT float collider_rotation_impl(Collider* collider);
SOREN_EXPORT void collider_set_rotation_impl(Collider* collider, float rotation);

SOREN_EXPORT float collider_scale_impl(Collider* collider);
SOREN_EXPORT void collider_set_scale_impl(Collider* collider, float scale);

SOREN_EXPORT Vector collider_position_impl(Collider* collider);
SOREN_EXPORT void collider_set_position_impl(Collider* collider, Vector position);

SOREN_EXPORT RectF collider_bounds_impl(Collider* collider);

SOREN_EXPORT bool collider_overlaps_rect_impl(Collider* collider, RectF rect);
SOREN_EXPORT bool collider_overlaps_collider_impl(Collider* collider, Collider* other);
SOREN_EXPORT bool collider_overlaps_line_impl(Collider* collider, Vector start, Vector end);
SOREN_EXPORT bool collider_contains_point_impl(Collider* collider, Vector point);
SOREN_EXPORT bool collider_collides_rect_impl(Collider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool collider_collides_collider_impl(Collider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool collider_collides_line_impl(Collider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool collider_collides_point_impl(Collider* collider, Vector point, CollisionResult* out_result);

SOREN_EXPORT CircleCollider* circle_collider_create(float radius);
SOREN_EXPORT void circle_collider_init(CircleCollider* circle, float radius);

SOREN_EXPORT float circle_collider_rotation(CircleCollider* circle);
SOREN_EXPORT void circle_collider_set_rotation(CircleCollider* circle, float rotation);

SOREN_EXPORT float circle_collider_scale(CircleCollider* circle);
SOREN_EXPORT void circle_collider_set_scale(CircleCollider* circle, float scale);

SOREN_EXPORT Vector circle_collider_position(CircleCollider* circle);
SOREN_EXPORT void circle_collider_set_position(CircleCollider* circle, Vector position);

SOREN_EXPORT RectF circle_collider_bounds(CircleCollider* circle);

SOREN_EXPORT float circle_collider_radius(CircleCollider* circle);

SOREN_EXPORT float circle_collider_original_radius(CircleCollider* circle);
SOREN_EXPORT void circle_collider_set_original_radius(CircleCollider* circle, float value);

SOREN_EXPORT bool circle_collider_overlaps_rect(CircleCollider* collider, RectF rect);
SOREN_EXPORT bool circle_collider_overlaps_collider(CircleCollider* collider, Collider* other);
SOREN_EXPORT bool circle_collider_overlaps_line(CircleCollider* collider, Vector start, Vector end);
SOREN_EXPORT bool circle_collider_contains_point(CircleCollider* collider, Vector point);
SOREN_EXPORT bool circle_collider_collides_rect(CircleCollider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool circle_collider_collides_collider(CircleCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool circle_collider_collides_line(CircleCollider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool circle_collider_collides_point(CircleCollider* collider, Vector point, CollisionResult* out_result);

SOREN_EXPORT PolygonCollider* polygon_collider_create(Vector* points, int count);
SOREN_EXPORT void polygon_collider_init(PolygonCollider* polygon, Vector* points, int count);

SOREN_EXPORT float polygon_collider_rotation(PolygonCollider* polygon);
SOREN_EXPORT void polygon_collider_set_rotation(PolygonCollider* polygon, float rotation);

SOREN_EXPORT float polygon_collider_scale(PolygonCollider* polygon);
SOREN_EXPORT void polygon_collider_set_scale(PolygonCollider* polygon, float scale);

SOREN_EXPORT Vector polygon_collider_position(PolygonCollider* polygon);
SOREN_EXPORT void polygon_collider_set_position(PolygonCollider* polygon, Vector position);

SOREN_EXPORT Vector polygon_collider_center(PolygonCollider* polygon);

SOREN_EXPORT Vector polygon_collider_original_center(PolygonCollider* polygon);
SOREN_EXPORT void polygon_collider_set_original_center(PolygonCollider* polygon, Vector center);

SOREN_EXPORT Vector* polygon_collider_points(PolygonCollider* polygon, int* out_count);

SOREN_EXPORT RectF polygon_collider_bounds(PolygonCollider* polygon);

SOREN_EXPORT Vector* polygon_collider_edge_normals(PolygonCollider* polygon, int* out_count);

SOREN_EXPORT bool polygon_collider_overlaps_rect(PolygonCollider* collider, RectF rect);
SOREN_EXPORT bool polygon_collider_overlaps_collider(PolygonCollider* collider, Collider* other);
SOREN_EXPORT bool polygon_collider_overlaps_line(PolygonCollider* collider, Vector start, Vector end);
SOREN_EXPORT bool polygon_collider_contains_point(PolygonCollider* collider, Vector point);
SOREN_EXPORT bool polygon_collider_collides_rect(PolygonCollider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool polygon_collider_collides_collider(PolygonCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool polygon_collider_collides_line(PolygonCollider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool polygon_collider_collides_point(PolygonCollider* collider, Vector point, CollisionResult* out_result);

SOREN_EXPORT BoxCollider* box_collider_create(float width, float height);
SOREN_EXPORT void box_collider_init(BoxCollider* collider, float width, float height);

SOREN_EXPORT float box_collider_rotation(BoxCollider* box);
SOREN_EXPORT void box_collider_set_rotation(BoxCollider* box, float rotation);

SOREN_EXPORT float box_collider_scale(BoxCollider* box);
SOREN_EXPORT void box_collider_set_scale(BoxCollider* box, float scale);

SOREN_EXPORT Vector box_collider_position(BoxCollider* box);
SOREN_EXPORT void box_collider_set_position(BoxCollider* box, Vector position);

SOREN_EXPORT Vector box_collider_center(BoxCollider* box);

SOREN_EXPORT Vector box_collider_original_center(BoxCollider* box);
SOREN_EXPORT void box_collider_set_original_center(BoxCollider* box, Vector center);

SOREN_EXPORT Vector box_collider_size(BoxCollider* box);

SOREN_EXPORT float box_collider_get_width(BoxCollider* box);

SOREN_EXPORT float box_collider_get_height(BoxCollider* box);

SOREN_EXPORT Vector box_collider_original_size(BoxCollider* box);
SOREN_EXPORT void box_collider_original_set_size(BoxCollider* box, Vector size);

SOREN_EXPORT float box_collider_original_get_width(BoxCollider* box);
SOREN_EXPORT void box_collider_original_set_width(BoxCollider* box, float width);

SOREN_EXPORT float box_collider_original_get_height(BoxCollider* box);
SOREN_EXPORT void box_collider_original_set_height(BoxCollider* box, float height);

SOREN_EXPORT Vector* box_collider_points(BoxCollider* box, int* out_count);

SOREN_EXPORT RectF box_collider_bounds(BoxCollider* box);

SOREN_EXPORT Vector* box_collider_edge_normals(BoxCollider* box, int* out_count);

SOREN_EXPORT bool box_collider_overlaps_rect(BoxCollider* collider, RectF rect);
SOREN_EXPORT bool box_collider_overlaps_collider(BoxCollider* collider, Collider* other);
SOREN_EXPORT bool box_collider_overlaps_line(BoxCollider* collider, Vector start, Vector end);
SOREN_EXPORT bool box_collider_contains_point(BoxCollider* collider, Vector point);
SOREN_EXPORT bool box_collider_collides_rect(BoxCollider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool box_collider_collides_collider(BoxCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool box_collider_collides_line(BoxCollider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool box_collider_collides_point(BoxCollider* collider, Vector point, CollisionResult* out_result);

SOREN_EXPORT float line_collider_rotation(LineCollider* line);
SOREN_EXPORT void line_collider_set_rotation(LineCollider* line, float rotation);

SOREN_EXPORT float line_collider_scale(LineCollider* line);
SOREN_EXPORT void line_collider_set_scale(LineCollider* line, float scale);

SOREN_EXPORT Vector line_collider_position(LineCollider* line);
SOREN_EXPORT void line_collider_set_position(LineCollider* line, Vector position);

SOREN_EXPORT RectF line_collider_bounds(LineCollider* line);

SOREN_EXPORT Vector line_collider_start(LineCollider* line);

SOREN_EXPORT Vector line_collider_end(LineCollider* line);

SOREN_EXPORT Vector line_collider_pivot(LineCollider* line);

SOREN_EXPORT Vector line_collider_adjusted_start(LineCollider* line);

SOREN_EXPORT Vector line_collider_adjusted_end(LineCollider* line);

SOREN_EXPORT Vector line_collider_original_start(LineCollider* line);
SOREN_EXPORT void line_collider_set_original_start(LineCollider* line, Vector start);

SOREN_EXPORT Vector line_collider_original_end(LineCollider* line);
SOREN_EXPORT void line_collider_set_original_end(LineCollider* line, Vector end);

SOREN_EXPORT Vector line_collider_original_pivot(LineCollider* line);
SOREN_EXPORT void line_collider_set_original_pivot(LineCollider* line, Vector pivot);

SOREN_EXPORT bool line_collider_overlaps_rect(LineCollider* collider, RectF rect);
SOREN_EXPORT bool line_collider_overlaps_collider(LineCollider* collider, Collider* other);
SOREN_EXPORT bool line_collider_overlaps_line(LineCollider* collider, Vector start, Vector end);
SOREN_EXPORT bool line_collider_contains_point(LineCollider* collider, Vector point);
SOREN_EXPORT bool line_collider_collides_rect(LineCollider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool line_collider_collides_collider(LineCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool line_collider_collides_line(LineCollider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool line_collider_collides_point(LineCollider* collider, Vector point, CollisionResult* out_result);

SOREN_EXPORT float point_collider_rotation(PointCollider* point);
SOREN_EXPORT void point_collider_set_rotation(PointCollider* point, float rotation);

SOREN_EXPORT float point_collider_scale(PointCollider* point);
SOREN_EXPORT void point_collider_set_scale(PointCollider* point, float scale);

SOREN_EXPORT Vector point_collider_position(PointCollider* point);
SOREN_EXPORT void point_collider_set_position(PointCollider* point, Vector position);

SOREN_EXPORT RectF point_collider_bounds(PointCollider* point);

SOREN_EXPORT bool point_collider_overlaps_rect(PointCollider* collider, RectF rect);
SOREN_EXPORT bool point_collider_overlaps_collider(PointCollider* collider, Collider* other);
SOREN_EXPORT bool point_collider_overlaps_line(PointCollider* collider, Vector start, Vector end);
SOREN_EXPORT bool point_collider_contains_point(PointCollider* collider, Vector point);
SOREN_EXPORT bool point_collider_collides_rect(PointCollider* collider, RectF rect, CollisionResult* out_result);
SOREN_EXPORT bool point_collider_collides_collider(PointCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
SOREN_EXPORT bool point_collider_collides_line(PointCollider* collider, Vector start, Vector end, RaycastHit* out_result);
SOREN_EXPORT bool point_collider_collides_point(PointCollider* collider, Vector point, CollisionResult* out_result);

static inline bool point_collider_using_internal_collider(PointCollider* point) {
    return point_collider_scale(point) == 1;
}

#define collider_rotation(collider) \
    _Generic((collider), \
        Collider*: collider_rotation_impl, \
        PointCollider*: point_collider_rotation, \
        LineCollider*: line_collider_rotation, \
        CircleCollider*: circle_collider_rotation, \
        PolygonCollider*: polygon_collider_rotation, \
        BoxCollider*: box_collider_rotation \
    )(collider)

#define collider_set_rotation(collider, rotation) \
    _Generic((collider), \
        Collider*: collider_set_rotation_impl, \
        PointCollider*: point_collider_set_rotation, \
        LineCollider*: line_collider_set_rotation, \
        CircleCollider*: circle_collider_set_rotation, \
        PolygonCollider*: polygon_collider_set_rotation, \
        BoxCollider*: box_collider_set_rotation \
    )((collider), (rotation))

#define collider_scale(collider) \
    _Generic((collider), \
        Collider*: collider_scale_impl, \
        PointCollider*: point_collider_scale, \
        LineCollider*: line_collider_scale, \
        CircleCollider*: circle_collider_scale, \
        PolygonCollider*: polygon_collider_scale, \
        BoxCollider*: box_collider_scale \
    )(collider)

#define collider_set_scale(collider, scale) \
    _Generic((collider), \
        Collider*: collider_set_scale_impl, \
        PointCollider*: point_collider_set_scale, \
        LineCollider*: line_collider_set_scale, \
        CircleCollider*: circle_collider_set_scale, \
        PolygonCollider*: polygon_collider_set_scale, \
        BoxCollider*: box_collider_set_scale \
    )((collider), (scale))

#define collider_position(collider) \
    _Generic((collider), \
        Collider*: collider_position_impl, \
        PointCollider*: point_collider_position, \
        LineCollider*: line_collider_position, \
        CircleCollider*: circle_collider_position, \
        PolygonCollider*: polygon_collider_position, \
        BoxCollider*: box_collider_position \
    )(collider)

#define collider_set_position(collider, position) \
    _Generic((collider), \
        Collider*: collider_set_position_impl, \
        PointCollider*: point_collider_set_position, \
        LineCollider*: line_collider_set_position, \
        CircleCollider*: circle_collider_set_position, \
        PolygonCollider*: polygon_collider_set_position, \
        BoxCollider*: box_collider_set_position \
    )((collider), (position))

#define collider_bounds(collider) \
    _Generic((collider), \
        Collider*: collider_bounds_impl, \
        PointCollider*: point_collider_bounds, \
        LineCollider*: line_collider_bounds, \
        CircleCollider*: circle_collider_bounds, \
        PolygonCollider*: polygon_collider_bounds, \
        BoxCollider*: box_collider_bounds \
    )(collider)


#define SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(arg1, ...) arg1

// This is a little wonky. Basically it check if there are any arguments passed to the macro,
// and if so, it chooses collider_overlaps_line_impl. Otherwise it chooses collider_contains_point_impl.
#define SOREN_CIRCLE_COLLIDER_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(circle_collider_overlaps_line,) circle_collider_contains_point)

#define circle_collider_overlaps(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: circle_collider_overlaps_collider, \
        RectF: circle_collider_overlaps_rect, \
        Vector: SOREN_CIRCLE_COLLIDER_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define circle_collider_collides(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: circle_collider_collides_rect, \
        Vector: _Generic((arg2), \
            Vector: circle_collider_collides_line, \
            default: circle_collider_collides_point) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define SOREN_BOX_COLLIDER_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(box_collider_overlaps_line,) box_collider_contains_point)

#define box_collider_overlaps(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: box_collider_overlaps_collider, \
        RectF: box_collider_overlaps_rect, \
        Vector: SOREN_BOX_COLLIDER_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define box_collider_collides(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: box_collider_collides_rect, \
        Vector: _Generic((arg2), \
            Vector: box_collider_collides_line, \
            default: box_collider_collides_point) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define SOREN_POLYGON_COLLIDER_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(polygon_collider_overlaps_line,) polygon_collider_contains_point)

#define polygon_collider_overlaps(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: polygon_collider_overlaps_collider, \
        RectF: polygon_collider_overlaps_rect, \
        Vector: SOREN_POLYGON_COLLIDER_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define polygon_collider_collides(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: polygon_collider_collides_rect, \
        Vector: _Generic((arg2), \
            Vector: polygon_collider_collides_line, \
            default: polygon_collider_collides_point) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define SOREN_LINE_COLLIDER_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(line_collider_overlaps_line,) line_collider_contains_point)

#define line_collider_overlaps(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: line_collider_overlaps_collider, \
        RectF: line_collider_overlaps_rect, \
        Vector: SOREN_LINE_COLLIDER_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define line_collider_collides(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: line_collider_collides_rect, \
        Vector: _Generic((arg2), \
            Vector: line_collider_collides_line, \
            default: line_collider_collides_point) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define SOREN_POINT_COLLIDER_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(point_collider_overlaps_line,) point_collider_contains_point)

#define point_collider_overlaps(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: point_collider_overlaps_collider, \
        RectF: point_collider_overlaps_rect, \
        Vector: SOREN_POINT_COLLIDER_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define point_collider_collides(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: point_collider_collides_rect, \
        Vector: _Generic((arg2), \
            Vector: point_collider_collides_line, \
            default: point_collider_collides_point) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define SOREN_COLLIDER_IMPL_OVERLAPS_CHOOSER(...) \
    SOREN_COLLIDER_OVERLAPS_GET_FIRST_ARG(__VA_OPT__(collider_overlaps_line_impl,) collider_contains_point_impl)

#define collider_overlaps_impl(collider, arg1, ...) \
    _Generic((arg1), \
        Collider*: collider_overlaps_collider_impl, \
        RectF: collider_overlaps_rect_impl, \
        Vector: SOREN_COLLIDER_IMPL_OVERLAPS_CHOOSER(__VA_ARGS__) \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define collider_collides_impl(collider, arg1, arg2, ...) \
    _Generic((arg1), \
        Collider*: cicle_collider_collides_collider, \
        RectF: collider_collides_rect_impl, \
        Vector: _Generic((arg2), \
            Vector: collider_collides_line_impl, \
            default: collider_collides_point_impl) \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#define collider_overlaps(collider, arg1, ...) \
    _Generic((collider), \
        Collider*: collider_overlaps_impl, \
        PointCollider*: point_collider_overlaps, \
        LineCollider*: line_collider_overlaps, \
        CircleCollider*: circle_collider_overlaps, \
        PolygonCollider*: polygon_collider_overlaps, \
        BoxCollider*: box_collider_overlaps \
    )((collider), (arg1) __VA_OPT__(,) __VA_ARGS__)

#define collider_collides(collider, arg1, arg2, ...) \
    _Generic((collider), \
        Collider*: collider_collides_impl, \
        PointCollider*: point_collider_collides, \
        LineCollider*: line_collider_collides, \
        CircleCollider*: circle_collider_collides, \
        PolygonCollider*: polygon_collider_collides, \
        BoxCollider*: box_collider_collides \
    )((collider), (arg1), (arg2) __VA_OPT__(,) __VA_ARGS__)

#endif