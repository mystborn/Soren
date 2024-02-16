#ifndef SOREN_COLLISIONS_SOREN_COLLIDERS_H
#define SOREN_COLLISIONS_SOREN_COLLIDERS_H

#include <stdbool.h>

#include <SDL3/SDL.h>

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

void collider_init(Collider* collider, ColliderType type);
void collider_free_resources(Collider* collider);
void collider_free(Collider* collider);

float collider_rotation_impl(Collider* collider);
void collider_set_rotation_impl(Collider* collider, float rotation);

float collider_scale_impl(Collider* collider);
void collider_set_scale_impl(Collider* collider, float scale);

Vector collider_position_impl(Collider* collider);
void collider_set_position_impl(Collider* collider, Vector position);

RectF collider_bounds_impl(Collider* collider);

bool collider_overlaps_rect_impl(Collider* collider, RectF rect);
bool collider_overlaps_collider_impl(Collider* collider, Collider* other);
bool collider_overlaps_line_impl(Collider* collider, Vector start, Vector end);
bool collider_contains_point_impl(Collider* collider, Vector point);
bool collider_collides_rect_impl(Collider* collider, RectF rect, CollisionResult* out_result);
bool collider_collides_collider_impl(Collider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool collider_collides_line_impl(Collider* collider, Vector start, Vector end, RaycastHit* out_result);
bool collider_collides_point_impl(Collider* collider, Vector point, CollisionResult* out_result);

CircleCollider* circle_collider_create(float radius);
void circle_collider_init(CircleCollider* circle, float radius);

float circle_collider_rotation(CircleCollider* circle);
void circle_collider_set_rotation(CircleCollider* circle, float rotation);

float circle_collider_scale(CircleCollider* circle);
void circle_collider_set_scale(CircleCollider* circle, float scale);

Vector circle_collider_position(CircleCollider* circle);
void circle_collider_set_position(CircleCollider* circle, Vector position);

RectF circle_collider_bounds(CircleCollider* circle);

float circle_collider_radius(CircleCollider* circle);

float circle_collider_original_radius(CircleCollider* circle);
void circle_collider_set_original_radius(CircleCollider* circle, float value);

bool circle_collider_overlaps_rect(CircleCollider* collider, RectF rect);
bool circle_collider_overlaps_collider(CircleCollider* collider, Collider* other);
bool circle_collider_overlaps_line(CircleCollider* collider, Vector start, Vector end);
bool circle_collider_contains_point(CircleCollider* collider, Vector point);
bool circle_collider_collides_rect(CircleCollider* collider, RectF rect, CollisionResult* out_result);
bool circle_collider_collides_collider(CircleCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool circle_collider_collides_line(CircleCollider* collider, Vector start, Vector end, RaycastHit* out_result);
bool circle_collider_collides_point(CircleCollider* collider, Vector point, CollisionResult* out_result);

PolygonCollider* polygon_collider_create(Vector* points, int count);
void polygon_collider_init(PolygonCollider* polygon, Vector* points, int count);

float polygon_collider_rotation(PolygonCollider* polygon);
void polygon_collider_set_rotation(PolygonCollider* polygon, float rotation);

float polygon_collider_scale(PolygonCollider* polygon);
void polygon_collider_set_scale(PolygonCollider* polygon, float scale);

Vector polygon_collider_position(PolygonCollider* polygon);
void polygon_collider_set_position(PolygonCollider* polygon, Vector position);

Vector polygon_collider_center(PolygonCollider* polygon);

Vector polygon_collider_original_center(PolygonCollider* polygon);
void polygon_collider_set_original_center(PolygonCollider* polygon, Vector center);

Vector* polygon_collider_points(PolygonCollider* polygon, int* out_count);

RectF polygon_collider_bounds(PolygonCollider* polygon);

Vector* polygon_collider_edge_normals(PolygonCollider* polygon, int* out_count);

bool polygon_collider_overlaps_rect(PolygonCollider* collider, RectF rect);
bool polygon_collider_overlaps_collider(PolygonCollider* collider, Collider* other);
bool polygon_collider_overlaps_line(PolygonCollider* collider, Vector start, Vector end);
bool polygon_collider_contains_point(PolygonCollider* collider, Vector point);
bool polygon_collider_collides_rect(PolygonCollider* collider, RectF rect, CollisionResult* out_result);
bool polygon_collider_collides_collider(PolygonCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool polygon_collider_collides_line(PolygonCollider* collider, Vector start, Vector end, RaycastHit* out_result);
bool polygon_collider_collides_point(PolygonCollider* collider, Vector point, CollisionResult* out_result);

BoxCollider* box_collider_create(float width, float height);
void box_collider_init(BoxCollider* collider, float width, float height);

float box_collider_rotation(BoxCollider* box);
void box_collider_set_rotation(BoxCollider* box, float rotation);

float box_collider_scale(BoxCollider* box);
void box_collider_set_scale(BoxCollider* box, float scale);

Vector box_collider_position(BoxCollider* box);
void box_collider_set_position(BoxCollider* box, Vector position);

Vector box_collider_center(BoxCollider* box);

Vector box_collider_original_center(BoxCollider* box);
void box_collider_set_original_center(BoxCollider* box, Vector center);

Vector box_collider_size(BoxCollider* box);

float box_collider_get_width(BoxCollider* box);

float box_collider_get_height(BoxCollider* box);

Vector box_collider_original_size(BoxCollider* box);
void box_collider_original_set_size(BoxCollider* box, Vector size);

float box_collider_original_get_width(BoxCollider* box);
void box_collider_original_set_width(BoxCollider* box, float width);

float box_collider_original_get_height(BoxCollider* box);
void box_collider_original_set_height(BoxCollider* box, float height);

Vector* box_collider_points(BoxCollider* box, int* out_count);

RectF box_collider_bounds(BoxCollider* box);

Vector* box_collider_edge_normals(BoxCollider* box, int* out_count);

bool box_collider_overlaps_rect(BoxCollider* collider, RectF rect);
bool box_collider_overlaps_collider(BoxCollider* collider, Collider* other);
bool box_collider_overlaps_line(BoxCollider* collider, Vector start, Vector end);
bool box_collider_contains_point(BoxCollider* collider, Vector point);
bool box_collider_collides_rect(BoxCollider* collider, RectF rect, CollisionResult* out_result);
bool box_collider_collides_collider(BoxCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool box_collider_collides_line(BoxCollider* collider, Vector start, Vector end, RaycastHit* out_result);
bool box_collider_collides_point(BoxCollider* collider, Vector point, CollisionResult* out_result);

float line_collider_rotation(LineCollider* line);
void line_collider_set_rotation(LineCollider* line, float rotation);

float line_collider_scale(LineCollider* line);
void line_collider_set_scale(LineCollider* line, float scale);

Vector line_collider_position(LineCollider* line);
void line_collider_set_position(LineCollider* line, Vector position);

RectF line_collider_bounds(LineCollider* line);

Vector line_collider_start(LineCollider* line);

Vector line_collider_end(LineCollider* line);

Vector line_collider_pivot(LineCollider* line);

Vector line_collider_adjusted_start(LineCollider* line);

Vector line_collider_adjusted_end(LineCollider* line);

Vector line_collider_original_start(LineCollider* line);
void line_collider_set_original_start(LineCollider* line, Vector start);

Vector line_collider_original_end(LineCollider* line);
void line_collider_set_original_end(LineCollider* line, Vector end);

Vector line_collider_original_pivot(LineCollider* line);
void line_collider_set_original_pivot(LineCollider* line, Vector pivot);

bool line_collider_overlaps_rect(LineCollider* collider, RectF rect);
bool line_collider_overlaps_collider(LineCollider* collider, Collider* other);
bool line_collider_overlaps_line(LineCollider* collider, Vector start, Vector end);
bool line_collider_contains_point(LineCollider* collider, Vector point);
bool line_collider_collides_rect(LineCollider* collider, RectF rect, CollisionResult* out_result);
bool line_collider_collides_collider(LineCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool line_collider_collides_line(LineCollider* collider, Vector start, Vector end, RaycastHit* out_result);
bool line_collider_collides_point(LineCollider* collider, Vector point, CollisionResult* out_result);

float point_collider_rotation(PointCollider* point);
void point_collider_set_rotation(PointCollider* point, float rotation);

float point_collider_scale(PointCollider* point);
void point_collider_set_scale(PointCollider* point, float scale);

Vector point_collider_position(PointCollider* point);
void point_collider_set_position(PointCollider* point, Vector position);

RectF point_collider_bounds(PointCollider* point);

bool point_collider_overlaps_rect(PointCollider* collider, RectF rect);
bool point_collider_overlaps_collider(PointCollider* collider, Collider* other);
bool point_collider_overlaps_line(PointCollider* collider, Vector start, Vector end);
bool point_collider_contains_point(PointCollider* collider, Vector point);
bool point_collider_collides_rect(PointCollider* collider, RectF rect, CollisionResult* out_result);
bool point_collider_collides_collider(PointCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit);
bool point_collider_collides_line(PointCollider* collider, Vector start, Vector end, RaycastHit* out_result);
bool point_collider_collides_point(PointCollider* collider, Vector point, CollisionResult* out_result);

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