#ifndef SOREN_MATH_H
#define SOREN_MATH_H

#include "soren_std.h"

#include <SDL3/SDL.h>

#include <sso_string.h>
#include "external/SFMT.h"

typedef SDL_FPoint Vector;
typedef SDL_Point Point;
typedef SDL_Rect Rect;
typedef SDL_FRect RectF;
typedef sfmt_t Random;

#define VECTOR_ZERO \
    (Vector){ 0, 0 }

#define VECTOR_ONE \
    (Vector){ 1, 1 }

#define POINT_ZERO \
    (Point){ 0, 0 }

#define POINT_ONE \
    (Point){ 1, 1 }

#define RECT_EMPTY \
    (Rect){ 0 }

#define RECTF_EMPTY \
    (RectF){ 0 }

typedef struct Matrix {
    float m11;
    float m12;
    float m21;
    float m22;
    float m31;
    float m32;
} Matrix;

#define MATRIX_IDENTITY \
    (Matrix){ 1.f, 0, 0, 1.f, 0, 0 }

// Todo: Rest of min_of_ functions

static inline int32_t min_of_i32(int32_t* values, int count) {
    if (count == 0) {
        return 0;
    }

    int32_t min = values[0];
    for (int i = 1; i < count; i++) {
        min = values[i] < min ? values[i] : min;
    }

    return min;
}

static inline float min_of_f32(float* values, int count) {
    if (count == 0) {
        return 0;
    }

    float min = values[0];
    for (int i = 1; i < count; i++) {
        min = values[i] < min ? values[i] : min;
    }

    return min;
}

#define soren_abs(value) \
    ((value) < 0 ? (value) : (value) * -1)

static inline float radians_to_degrees(float radians) {
    return (float)(radians * 57.2957795130322320876798154814105);
}

static inline float degrees_to_radians(float degrees) {
    return (float)(degrees * 0.017453292519943295769236907684886);
}

static inline Vector vector_create(float x, float y) {
    return (Vector){ x, y };
}

static inline Vector vector_subtract(Vector left, Vector right) {
    return (Vector) {
        .x = left.x - right.x,
        .y = left.y - right.y
    };
}

static inline Vector vector_add(Vector left, Vector right) {
    return (Vector) {
        .x = left.x + right.x,
        .y = left.y + right.y
    };
}

static inline Vector vector_multiply(Vector left, Vector right) {
    return (Vector) {
        .x = left.x * right.x,
        .y = left.y * right.y
    };
}

static inline Vector vector_divide(Vector left, Vector right) {
    return (Vector) {
        .x = left.x / right.x,
        .y = left.y / right.y
    };
}

static inline Vector vector_multiply_scalar(Vector left, float scalar) {
    return (Vector) {
        .x = left.x * scalar,
        .y = left.y * scalar
    };
}

static inline Vector vector_divide_scalar(Vector left, float scalar) {
    return (Vector) {
        .x = left.x / scalar,
        .y = left.y / scalar
    };
}

static inline bool vector_equals(Vector left, Vector right) {
    return left.x == right.x
        && left.y == right.y;
}

static inline Vector vector_round(Vector vector) {
    return (Vector) {
        .x = SDL_roundf(vector.x),
        .y = SDL_roundf(vector.y)
    };
}

static inline Vector vector_floor(Vector vector) {
    return (Vector) {
        .x = SDL_floorf(vector.x),
        .y = SDL_floorf(vector.y)
    };
}

static inline Vector vector_ceil(Vector vector) {
    return (Vector) {
        .x = SDL_ceilf(vector.x),
        .y = SDL_ceilf(vector.y)
    };
}

static inline Vector vector_clamp(Vector vector, Vector min, Vector max) {
    return (Vector) {
        .x = SDL_clamp(vector.x, min.x, max.x),
        .y = SDL_clamp(vector.y, min.y, max.y)
    };
}

static inline float vector_distance(Vector left, Vector right) {
    float x = left.x - right.x;
    float y = left.y - right.y;
    return SDL_sqrtf(x * x + y * y);
}

static inline float vector_distance_squared(Vector left, Vector right) {
    float x = left.x - right.x;
    float y = left.y - right.y;
    return x * x + y * y;
}

static inline float vector_dot(Vector left, Vector right) {
    return left.x * right.x + left.y * right.y;
}

static inline float vector_cross(Vector left, Vector right) {
    return left.x * right.y - left.y * right.x;
}

static inline uint32_t vector_hash(Vector* vector) {
    // Generate a simple hash code by reinterpreting the
    // vector fields as ints, then combining them.
    uint32_t x = *(uint32_t*)&vector->x;
    uint32_t y = *(uint32_t*)&vector->y;
    return (x * 397) ^ y;
}

static inline float vector_length(Vector vector) {
    return SDL_sqrtf(vector.x * vector.x + vector.y * vector.y);
}

static inline float vector_length_squared(Vector vector) {
    return vector.x * vector.x + vector.y * vector.y;
}

static inline Vector vector_max(Vector left, Vector right) {
    return (Vector) {
        .x = SDL_max(left.x, right.x),
        .y = SDL_max(left.y, right.y)
    };
}

static inline Vector vector_min(Vector left, Vector right) {
    return (Vector) {
        .x = SDL_min(left.x, right.x),
        .y = SDL_min(left.y, right.y)
    };
}

static inline Vector vector_negate(Vector value) {
    return (Vector) {
        .x = -value.x,
        .y = -value.y
    };
}

static inline Vector vector_normalize(Vector value) {
    float ratio = 1.f / vector_length(value);
    value.x *= ratio;
    value.y *= ratio;

    return value;
}

/**
 * Creates a new vector that contains a reflection of the given vector and normal.
 * 
 * @param vector The vector to reflect.
 * @param normal The reflection normal of the axis to reflect over.
 * @return The reflected vector.
 */
static inline Vector vector_reflect(Vector vector, Vector normal) {
    float num = 2.f * vector_dot(vector, normal);
    return (Vector) {
        .x = vector.x - normal.x * num,
        .y = vector.y - normal.y * num
    };
}

static inline Vector vector_perpendicular(Vector start, Vector end) {
    return (Vector) {
        -1.f * (end.y - start.y),
        end.x - start.x 
    };
}

/**
 * Writes a vector to a string. If `result` is `NULL`, allocates a new
 * string for this operation.
 * 
 * @param vector The vector to convert to a string.
 * @param result The string to append to. If `NULL`, a new string is created.
 * @return `result` if not `NULL`, otherwise a pointer to the newly created 
 *         string.
 */
static inline String* vector_to_string(Vector vector, String* result) {
    return string_format(result, "Vector {x: %f, y: %f}", vector.x, vector.y);
}

static inline Point vector_to_point(Vector vector) {
    return (Point){ 
        .x = (int)vector.x,
        .y = (int)vector.y
    };
}

static inline Vector vector_transform(Vector vector, Matrix* matrix) {
    return (Vector) {
        .x = vector.x * matrix->m11 + vector.y * matrix->m21 + matrix->m31,
        .y = vector.x * matrix->m12 + vector.y * matrix->m22 + matrix->m32
    };
}

static inline void vector_transform_batch(
    Vector* vectors,
    int vectors_count,
    Vector* result,
    Matrix* matrix)
{
    soren_assert(vectors);
    soren_assert(result);

    for (int i = 0; i < vectors_count; i++) {
        result[i] = vector_transform(vectors[i], matrix);
    }
}

static inline Vector vector_transform_normal(Vector normal, Matrix* matrix) {
    return (Vector) {
        .x = normal.x * matrix->m11 + normal.y * matrix->m21,
        .y = normal.x * matrix->m12 + normal.y * matrix->m22
    };
}

static inline vector_transform_normal_batch(
    Vector* vectors,
    int vectors_count,
    Vector* result,
    Matrix* matrix)
{
    soren_assert(vectors);
    soren_assert(result);

    for (int i = 0; i < vectors_count; i++) {
        result[i] = vector_transform_normal(vectors[i], matrix);
    }
}

static inline Vector vector_rotate(Vector value, float radians) {
    float cos = SDL_cosf(radians);
    float sin = SDL_sinf(radians);

    return (Vector) {
        .x = value.x * cos - value.y * sin,
        .y = value.x * sin + value.y * cos
    };
}

static inline Vector vector_rotate_around(
    Vector value,
    Vector origin,
    float radians) 
{
    value = vector_subtract(value, origin);
    value = vector_rotate(value, radians);
    value = vector_add(value, origin);
    return value;
}

static inline uint32_t point_hash(Point point) {
    return (uint32_t)((point.x * 397) ^ point.y);
}

static inline bool point_equals(Point left, Point right) {
    return left.x == right.x && left.y == right.y;
}

static inline int point_compare(Point left, Point right) {
    if (left.x == right.x) {
        return left.y < right.y ? -1 : (left.y > right.y ? 1 : 0);
    } else {
        return left.x < right.x ? -1 : 1;
    }
}

static inline float rectf_left(RectF rect);
static inline float rectf_top(RectF rect);
static inline float rectf_right(RectF rect);
static inline float rectf_bottom(RectF rect);

static inline int rect_left(Rect rect) {
    return rect.x;
}

static inline int rect_top(Rect rect) {
    return rect.y;
}

static inline int rect_right(Rect rect) {
    return rect.x + rect.w;
}

static inline int rect_bottom(Rect rect) {
    return rect.y + rect.h;
}

static inline bool rect_is_empty(Rect rect) {
    return rect.x == 0 
        && rect.y == 0 
        && rect.w == 0
        && rect.h == 0;
}

static inline Point rect_location(Rect rect) {
    return (Point){ rect.x, rect.y };
}

static inline void rect_set_location(Rect* rect, Point location) {
    rect->x = location.x;
    rect->y = location.y;
}

static inline Point rect_size(Rect rect) {
    return (Point){ rect.w, rect.h };
}

static inline void rect_set_size(Rect* rect, Point size) {
    rect->w = size.x;
    rect->h = size.y;
}

static inline Point rect_center(Rect rect) {
    return (Point) {
        rect.x + rect.w / 2,
        rect.y + rect.h / 2
    };
}


static inline bool rect_contains_coords(Rect rect, int x, int y);
static inline bool rect_contains_point(Rect rect, Point point);
static inline bool rect_contains_vector(Rect rect, Vector point);
static inline bool rect_contains_rect(Rect outer, Rect inner);
static inline bool rect_contains_rectf(Rect outer, RectF inner);

#define rect_contains(rect, value, ...) \
    _Generic((value), \
        int: rect_contains_coords, \
        Point: rect_contains_point, \
        Vector: rect_contains_vector, \
        Rect: rect_contains_rect, \
        RectF: rect_contains_rectf \
    )((rect), (value) __VA_OPT__(,) __VA_ARGS__)

static inline bool rect_contains_coords(Rect rect, int x, int y) {
    return rect.x <= x 
        && rect.x + rect.w >= x 
        && rect.y <= y
        && rect.y + rect.h >= y; 
}

static inline bool rect_contains_point(Rect rect, Point point) {
    return rect_contains_coords(rect, point.x, point.y);
}

static inline bool rect_contains_vector(Rect rect, Vector point) {
    return rect_contains_coords(rect, (int)point.x, (int)point.y);
}

static inline bool rect_contains_rect(Rect outer, Rect inner) {
    return outer.x <= inner.x
        && rect_right(outer) >= rect_right(inner)
        && outer.y <= inner.y
        && rect_bottom(outer) >= rect_bottom(inner);
}

static inline bool rect_contains_rectf(Rect outer, RectF inner) {
    return outer.x <= inner.x
        && rect_right(outer) >= rectf_right(inner)
        && outer.y <= inner.y
        && rect_bottom(outer) >= rectf_bottom(inner);
}

static inline bool rect_intersects(Rect left, Rect right) {
    return right.x <= rect_right(left)
        && left.x <= rect_right(right)
        && right.y <= rect_bottom(left)
        && left.y <= rect_bottom(right);
}

static inline bool rect_contains_or_intersects(Rect outer, Rect inner) {
    return rect_contains_rect(outer, inner) || rect_intersects(outer, inner);
}

static inline Vector closest_point_on_rect_to_point(Rect rect, Vector point) {
    return vector_create(
        SDL_clamp(point.x, rect.x, rect_right(rect)),
        SDL_clamp(point.y, rect.y, rect_bottom(rect))
    );
}

static inline Vector closest_point_on_rect_border_to_point(Rect rect, Vector point, Vector* out_edge_normal) {
    Vector edge_normal = VECTOR_ZERO;
    Vector result = vector_create(
        SDL_clamp(point.x, rect.x, rect_right(rect)),
        SDL_clamp(point.y, rect.y, rect_bottom(rect))
    );

    if (rect_contains(rect, result)) {
        float deltas[4] = {
            result.x - rect.x,
            rect_right(rect) - result.x,
            result.y - rect.y,
            rect_bottom(rect) - result.y
        };

        float min = min_of_f32(deltas, 4);
        if (min == deltas[0]) {
            result.x = (float)rect.x;
            edge_normal.x = -1;
        } else if (min == deltas[1]) {
            result.x = (float)rect_right(rect);
            edge_normal.x = 1;
        } else if (min == deltas[2]) {
            result.y = (float)rect.y;
            edge_normal.y = -1;
        } else {
            result.y = (float)rect_bottom(rect);
            edge_normal.y = 1;
        }
    } else {
        if (result.x == rect.x) {
            edge_normal.x = -1;
        } else if (result.x == rect_right(rect)) {
            edge_normal.x = 1;
        } else if (result.y == rect.y) {
            edge_normal.y = -1;
        } else {
            edge_normal.y = 1;
        }
    }

    return result;
}

static inline bool rect_equals(Rect left, Rect right) {
    return left.x == right.x
        && left.y == right.y
        && left.w == right.w
        && left.h == right.h;
}


static inline uint32_t rect_hash(Rect rect) {
    uint32_t hash = 17;

    hash = hash * 23 + rect.x;
    hash = hash * 23 + rect.y;
    hash = hash * 23 + rect.w;
    hash = hash * 23 + rect.h;

    return hash;
}

static inline String* rect_to_string(Rect rect, String* str) {
    return string_format(str, "Rect { x: %d, y: %d, w: %d, h: %d }", rect.x, rect.y, rect.w, rect.h);
}

static inline RectF rect_to_rectf(Rect rect) {
    return (RectF) {
        (float)rect.x,
        (float)rect.y,
        (float)rect.w,
        (float)rect.h
    };
}

static inline float rectf_left(RectF rect) {
    return rect.x;
}

static inline float rectf_top(RectF rect) {
    return rect.y;
}

static inline float rectf_right(RectF rect) {
    return rect.x + rect.w;
}

static inline float rectf_bottom(RectF rect) {
    return rect.y + rect.h;
}

static inline bool rectf_is_empty(RectF rect) {
    return rect.x == 0 
        && rect.y == 0 
        && rect.w == 0
        && rect.h == 0;
}

static inline Vector rectf_location(RectF rect) {
    return (Vector){ rect.x, rect.y };
}

static inline void rectf_set_location(RectF* rect, Vector location) {
    rect->x = location.x;
    rect->y = location.y;
}

static inline Vector rectf_size(RectF rect) {
    return (Vector){ rect.w, rect.h };
}

static inline void rectf_set_size(RectF* rect, Vector size) {
    rect->w = size.x;
    rect->h = size.y;
}

static inline Vector rectf_center(RectF rect) {
    return (Vector) {
        rect.x + rect.w / 2,
        rect.y + rect.h / 2
    };
}

static inline void rectf_points(RectF rect, Vector* points) {
    points[0].x = rect.x;
    points[0].y = rect.y;
    points[1].x = rect.x + rect.w;
    points[1].y = rect.y;
    points[2].x = rect.x + rect.w;
    points[2].y = rect.y + rect.h;
    points[3].x = rect.x;
    points[3].y = rect.y + rect.h;
}

static inline void rectf_vertices(RectF rect, SDL_Vertex* points) {
    points[0].position.x = rect.x;
    points[0].position.y = rect.y;
    points[1].position.x = rect.x + rect.w;
    points[1].position.y = rect.y;
    points[2].position.x = rect.x + rect.w;
    points[2].position.y = rect.y + rect.h;
    points[3].position.x = rect.x;
    points[3].position.y = rect.y + rect.h;
}

static inline bool rectf_contains_coords(RectF rect, float x, float y);
static inline bool rectf_contains_point(RectF rect, Point point);
static inline bool rectf_contains_vector(RectF rect, Vector point);
static inline bool rectf_contains_rect(RectF outer, Rect inner);
static inline bool rectf_contains_rectf(RectF outer, RectF inner);

#define rectf_contains(rect, value, ...) \
    _Generic((value), \
        float: rectf_contains_coords, \
        Point: rectf_contains_point, \
        Vector: rectf_contains_vector, \
        Rect: rectf_contains_rect, \
        RectF: rectf_contains_rectf \
    )((rect), (value) __VA_OPT__(,) __VA_ARGS__)

static inline bool rectf_contains_coords(RectF rect, float x, float y) {
    return rect.x <= x 
        && rect.x + rect.w >= x 
        && rect.y <= y
        && rect.y + rect.h >= y; 
}

static inline bool rectf_contains_point(RectF rect, Point point) {
    return rectf_contains(rect, (float)point.x, (float)point.y);
}

static inline bool rectf_contains_vector(RectF rect, Vector point) {
    return rectf_contains(rect, point.x, point.y);
}

static inline bool rectf_contains_rect(RectF outer, Rect inner) {
    return outer.x <= inner.x
        && rectf_right(outer) >= rect_right(inner)
        && outer.y <= inner.y
        && rectf_bottom(outer) >= rect_bottom(inner);
}

static inline bool rectf_contains_rectf(RectF outer, RectF inner) {
    return outer.x <= inner.x
        && rectf_right(outer) >= rectf_right(inner)
        && outer.y <= inner.y
        && rectf_bottom(outer) >= rectf_bottom(inner);
}

static inline bool rectf_intersects(RectF left, RectF right) {
    return right.x <= rectf_right(left)
        && left.x <= rectf_right(right)
        && right.y <= rectf_bottom(left)
        && left.y <= rectf_bottom(right);
}

static inline bool rectf_contains_or_intersects(RectF outer, RectF inner) {
    return rectf_contains_rectf(outer, inner) || rectf_intersects(outer, inner);
}

static inline Vector rectf_closest_point_on_bounds_to_origin(RectF rect) {
    Vector bottom_right = vector_create(rectf_right(rect), rectf_bottom(rect));
    float min_distance = soren_abs(rect.x);
    Vector bounds_point = vector_create(rect.x, 0);

    if (soren_abs(bottom_right.x) < min_distance) {
        min_distance = soren_abs(bottom_right.x);
        bounds_point.x = bottom_right.x;
        bounds_point.y = 0;
    }

    if (soren_abs(bottom_right.y) < min_distance) {
        min_distance = soren_abs(bottom_right.y);
        bounds_point.x = 0;
        bounds_point.y = bottom_right.y;
    }

    if (soren_abs(rect.y) < min_distance) {
        bounds_point.x = 0;
        bounds_point.y = rect.y;
    }

    return bounds_point;
}

static inline Vector closest_point_on_rectf_to_point(RectF rect, Vector point) {
    return vector_create(
        SDL_clamp(point.x, rect.x, rectf_right(rect)),
        SDL_clamp(point.y, rect.y, rectf_bottom(rect))
    );
}

static inline Vector closest_point_on_rectf_border_to_point(RectF rect, Vector point, Vector* out_edge_normal) {
    Vector edge_normal = VECTOR_ZERO;
    Vector result = vector_create(
        SDL_clamp(point.x, rect.x, rectf_right(rect)),
        SDL_clamp(point.y, rect.y, rectf_bottom(rect))
    );

    if (rectf_contains(rect, result)) {
        float deltas[] = {
            result.x - rect.x,
            rectf_right(rect) - result.x,
            result.y - rect.y,
            rectf_bottom(rect) - result.y
        };

        float min = min_of_f32(deltas, 4);
        if (min == deltas[0]) {
            result.x = rect.x;
            edge_normal.x = -1;
        } else if (min == deltas[1]) {
            result.x = rectf_right(rect);
            edge_normal.x = 1;
        } else if (min == deltas[2]) {
            result.y = rect.y;
            edge_normal.y = -1;
        } else {
            result.y = rectf_bottom(rect);
            edge_normal.y = 1;
        }
    } else {
        if (result.x == rect.x) {
            edge_normal.x = -1;
        } else if (result.x == rectf_right(rect)) {
            edge_normal.x = 1;
        } else if (result.y == rect.y) {
            edge_normal.y = -1;
        } else {
            edge_normal.y = 1;
        }
    }

    if (out_edge_normal) {
        *out_edge_normal = edge_normal;
    }

    return result;
}

static inline bool rectf_equals(RectF left, RectF right) {
    return left.x == right.x
        && left.y == right.y
        && left.w == right.w
        && left.h == right.h;
}


static inline uint32_t rectf_hash(RectF rect) {
    uint32_t hash = 17;

    hash = hash * 23 + *(uint32_t*)&rect.x;
    hash = hash * 23 + *(uint32_t*)&rect.y;
    hash = hash * 23 + *(uint32_t*)&rect.w;
    hash = hash * 23 + *(uint32_t*)&rect.h;

    return hash;
}

static inline String* rectf_to_string(RectF rect, String* str) {
    return string_format(str, "Rect { x: %f, y: %f, w: %f, h: %f }", rect.x, rect.y, rect.w, rect.h);
}

static inline Rect rectf_to_rect(RectF rect) {
    return (Rect) {
        (int)rect.x,
        (int)rect.y,
        (int)rect.w,
        (int)rect.h
    };
}

static inline void matrix_multiply(Matrix* left, Matrix* right, Matrix* result);

static inline Matrix matrix_create_rotation(float radians) {
    float cos = SDL_cosf(radians);
    float sin = SDL_sinf(radians);

    return (Matrix) {
        .m11 = cos,
        .m12 = sin,
        .m21 = -sin,
        .m22 = cos,
        .m31 = 0,
        .m32 = 0
    };
}

static inline Matrix matrix_create_scale(Vector scale) {
    return (Matrix) {
        .m11 = scale.x,
        .m12 = 0,
        .m21 = 0,
        .m22 = scale.y,
        .m31 = 0,
        .m32 = 0
    };
}

static inline Matrix matrix_create_translation(Vector translate) {
    return (Matrix) {
        .m11 = 1,
        .m12 = 0,
        .m21 = 0,
        .m22 = 1,
        .m31 = translate.x,
        .m32 = translate.y
    };
}

static inline Matrix matrix_create_tr(Vector translate, float rotation) {
    Matrix result = MATRIX_IDENTITY;
    if (rotation != 0) {
        Matrix rotation_matrix = matrix_create_rotation(rotation);
        matrix_multiply(&result, &rotation_matrix, &result);
    }

    if (!vector_equals(translate, VECTOR_ZERO)) {
        Matrix translate_matrix = matrix_create_translation(translate);
        matrix_multiply(&result, &translate_matrix, &result);
    }

    return result;
}

static inline Matrix matrix_create_tro(
    Vector translate,
    float rotation,
    Vector origin) 
{
    Matrix result = MATRIX_IDENTITY;
    result.m31 = -origin.x;
    result.m32 = -origin.y;

    if (rotation != 0) {
        Matrix rotation_matrix = matrix_create_rotation(rotation);
        matrix_multiply(&result, &rotation_matrix, &result);
    }

    if (!vector_equals(translate, VECTOR_ZERO)) {
        Matrix translate_matrix = matrix_create_translation(translate);
        matrix_multiply(&result, &translate_matrix, &result);
    }

    result.m31 += origin.x;
    result.m32 += origin.y;

    return result;
}

static inline Matrix matrix_create_trs(
    Vector translate,
    float rotation,
    Vector scale)
{
    Matrix result = MATRIX_IDENTITY;

    if (!vector_equals(scale, VECTOR_ONE)) {
        Matrix scale_matrix = matrix_create_scale(scale);
        matrix_multiply(&result, &scale_matrix, &result);
    }

    if (rotation != 0) {
        Matrix rotation_matrix = matrix_create_rotation(rotation);
        matrix_multiply(&result, &rotation_matrix, &result);
    }

    if (!vector_equals(translate, VECTOR_ZERO)) {
        Matrix translate_matrix = matrix_create_translation(translate);
        matrix_multiply(&result, &translate_matrix, &result);
    }

    return result;
}

static inline Matrix matrix_create_trso(
    Vector translate,
    float rotation,
    Vector scale,
    Vector origin)
{
    Matrix result = MATRIX_IDENTITY;

    result.m31 = -origin.x;
    result.m32 = -origin.y;

    if (!vector_equals(scale, VECTOR_ONE)) {
        Matrix scale_matrix = matrix_create_scale(scale);
        matrix_multiply(&result, &scale_matrix, &result);
    }

    if (rotation != 0) {
        Matrix rotation_matrix = matrix_create_rotation(rotation);
        matrix_multiply(&result, &rotation_matrix, &result);
    }

    if (!vector_equals(translate, VECTOR_ZERO)) {
        Matrix translate_matrix = matrix_create_translation(translate);
        matrix_multiply(&result, &translate_matrix, &result);
    }

    result.m31 += origin.x;
    result.m32 += origin.y;

    return result;
}

/**
 * Gets the translation component of a matrix.
 * 
 * @param matrix The matrix to get the translation component of.
 * @return The translation component of the matrix as a Vector.
 */
static inline Vector matrix_translation(Matrix* matrix) {
    return (Vector){ matrix->m31, matrix->m32 };
}

/**
 * Gets the rotation component of a matrix.
 * 
 * @param matrix The matrix to get the rotation component of.
 * @return The rotation component of the matrix as radians.
 */
static inline float matrix_rotation(Matrix* matrix) {
    return SDL_atan2f(matrix->m22, matrix->m11);
}

static inline Vector matrix_scale(Matrix* matrix) {
    return (Vector){
        .x = SDL_sqrtf(matrix->m11 * matrix->m11 + matrix->m21 + matrix->m21),
        .y = SDL_sqrtf(matrix->m12 * matrix->m12 + matrix->m22 + matrix->m22),
    };
}

static inline float matrix_determinant(Matrix* matrix) {
    return matrix->m11 * matrix->m22 - matrix->m12 * matrix->m21;
}

static inline void matrix_add(Matrix* left, Matrix* right, Matrix* result) {
    result->m11 = left->m11 + right->m11;
    result->m12 = left->m12 + right->m12;
    result->m21 = left->m21 + right->m21;
    result->m22 = left->m22 + right->m22;
    result->m31 = left->m31 + right->m31;
    result->m32 = left->m32 + right->m32;
}

static inline void matrix_subtract(Matrix* left, Matrix* right, Matrix* result) {
    result->m11 = left->m11 - right->m11;
    result->m12 = left->m12 - right->m12;
    result->m21 = left->m21 - right->m21;
    result->m22 = left->m22 - right->m22;
    result->m31 = left->m31 - right->m31;
    result->m32 = left->m32 - right->m32;
}

static inline void matrix_multiply(Matrix* left, Matrix* right, Matrix* result) {
    float m11 = left->m11 * right->m11 + left->m12 * right->m21;
    float m12 = left->m11 * right->m12 + left->m12 * right->m22;
    float m21 = left->m21 * right->m11 + left->m22 * right->m21;
    float m22 = left->m21 * right->m12 + left->m22 * right->m22;
    float m31 = left->m31 * right->m11 + left->m32 * right->m21 + right->m31;
    float m32 = left->m31 * right->m12 + left->m32 * right->m22 + right->m32;

    result->m11 = m11;
    result->m12 = m12;
    result->m21 = m21;
    result->m22 = m22;
    result->m31 = m31;
    result->m32 = m32;
}

static inline void matrix_multiply_scalar(
    Matrix* left,
    float right,
    Matrix* result)
{
    result->m11 = left->m11 * right;
    result->m12 = left->m12 * right;
    result->m21 = left->m21 * right;
    result->m22 = left->m22 * right;
    result->m31 = left->m31 * right;
    result->m32 = left->m32 * right;
}

static inline void matrix_divide(Matrix* left, Matrix* right, Matrix* result) {
    result->m11 = left->m11 / right->m11;
    result->m12 = left->m12 / right->m12;
    result->m21 = left->m21 / right->m21;
    result->m22 = left->m22 / right->m22;
    result->m31 = left->m31 / right->m31;
    result->m32 = left->m32 / right->m32;
}

static inline void matrix_divide_scalar(Matrix* left, float right, Matrix* result) {
    right = 1.f / right;
    matrix_multiply_scalar(left, right, result);
}

static inline void matrix_invert(Matrix* matrix) {
    float determinant = 1 / matrix_determinant(matrix);

     
    float m11 = matrix->m22 * determinant;
    float m12 = matrix->m12 * determinant;
    float m21 = matrix->m21 * determinant;
    float m22 = matrix->m11 * determinant;
    float m31 = (matrix->m32 * matrix->m21 - matrix->m31 * matrix->m22) * determinant;
    float m32 = -(matrix->m32 * matrix->m11 - matrix->m31 * matrix->m12) * determinant;

    matrix->m11 = m11;
    matrix->m12 = m12;
    matrix->m21 = m21;
    matrix->m22 = m22;
    matrix->m31 = m31;
    matrix->m32 = m32;
}

static inline bool matrix_equals(Matrix* left, Matrix* right) {
    return left->m11 == right->m11
        && left->m12 == right->m12
        && left->m21 == right->m21
        && left->m22 == right->m22
        && left->m31 == right->m31
        && left->m32 == right->m32;
}

static inline uint32_t matrix_hash(Matrix* matrix) {
    uint32_t hash = *(uint32_t*)&matrix->m11;
    hash = 31 * hash + *(uint32_t*)&matrix->m12;
    hash = 31 * hash + *(uint32_t*)&matrix->m21;
    hash = 31 * hash + *(uint32_t*)&matrix->m22;
    hash = 31 * hash + *(uint32_t*)&matrix->m31;
    hash = 31 * hash + *(uint32_t*)&matrix->m32;

    return hash;
}

static inline String* matrix_to_string(Matrix* matrix, String* result) {
    result = string_format(
        result,
        "Matrix {(m11: %f, m12: %f), (m21: %f, m22: %f), (m31: %f, m32: %f)}",
        matrix->m11,
        matrix->m12,
        matrix->m21,
        matrix->m22,
        matrix->m31,
        matrix->m32);

    return result;
}

static inline String* matrix_debug_string(Matrix* matrix, String* result) {
    if (matrix_equals(matrix, &MATRIX_IDENTITY)) {
        if (result) {
            string_append(result, "Matrix {Identity}");
        } else {
            result = string_create_ref("Identity");
        }
    } else {
        Vector translate = matrix_translation(matrix);
        float rotation = matrix_rotation(matrix);
        rotation = radians_to_degrees(rotation);
        Vector scale = matrix_scale(matrix);
        result = string_format(
            result,
            "Matrix {T: (%f, %f), R: %f, S: (%f, %f)}",
            translate.x,
            translate.y,
            rotation,
            scale.x,
            scale.y);
    }

    return result;
}

static inline int soren_sign_i32(int32_t value) {
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

static inline int soren_sign_i64(int64_t value) {
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

static inline int soren_sign_f32(float value) {
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

static inline int soren_sign_f64(double value) {
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

#define soren_sign(value) \
    _Generic((value), \
        int8_t: soren_sign_i32,\
        int16_t: soren_sign_i32, \
        int32_t: soren_sign_i32, \
        int64_t: soren_sign_i64, \
        float: soren_sign_f32, \
        double: soren_sign_f64 \
    )((value))

static inline void random_init(Random* random, uint32_t seed) {
    sfmt_init_gen_rand(random, seed);
}

static inline Random* random_create(uint32_t seed) {
    Random* random = soren_malloc(sizeof(*random));
    random_init(random, seed);
    return random;
}

static inline uint32_t random_u32(Random* random) {
    return sfmt_genrand_uint32(random);
}

static inline uint32_t random_u32_max(Random* random, int32_t max) {
    return sfmt_genrand_uint32(random) % (max + 1);
}

static inline uint32_t random_u32_range(Random* random, uint32_t start, uint32_t end) {
    return sfmt_genrand_uint32(random) % (end + 1 - start) + start;
}

static inline int32_t random_i32(Random* random) {
    return (int32_t)sfmt_genrand_uint32(random) - INT_MAX - 1;
}

static inline int32_t random_i32_max(Random* random, int32_t max) {
    return sfmt_genrand_uint32(random) % (max + 1);
}

static inline int32_t random_i32_range(Random* random, int32_t start, int32_t end) {
    return random_i32_max(random, end - start) + start;
}

static inline uint64_t random_u64(Random* random) {
    return sfmt_genrand_uint64(random);
}

static inline uint64_t random_u64_max(Random* random, int64_t max) {
    return sfmt_genrand_uint64(random) % (max + 1);
}

static inline uint64_t random_u64_range(Random* random, uint64_t start, uint64_t end) {
    return sfmt_genrand_uint64(random) % (end + 1 - start) + start;
}

static inline int64_t random_i64(Random* random) {
    return (int64_t)sfmt_genrand_uint64(random) - INT_MAX - 1;
}

static inline int64_t random_i64_max(Random* random, int64_t max) {
    return sfmt_genrand_uint64(random) % (max + 1);
}

static inline int64_t random_i64_range(Random* random, int64_t start, int64_t end) {
    return random_i64_max(random, end - start) + start;
}

static inline float random_float(Random* random) {
    return sfmt_genrand_real1(random);
}

static inline double random_double(Random* random) {
    return sfmt_genrand_res53(random);
}

#endif