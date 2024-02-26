#ifndef SOREN_COLLISIONS_SOREN_COLLISIONS_H
#define SOREN_COLLISIONS_SOREN_COLLISIONS_H

#include "soren_colliders.h"

typedef struct CollisionResult {
    Vector normal;
    Vector minimum_translation_vector;
    Vector point;
} CollisionResult;

typedef struct RaycastHit {
    float fraction;
    float distance;
    Vector point;
    Vector normal;
} RaycastHit;

SOREN_EXPORT void collision_result_remove_horizonal_translation(CollisionResult* result, Vector delta);
SOREN_EXPORT void collision_result_invert(CollisionResult* result);
SOREN_EXPORT void collision_result_to_raycast_hit(CollisionResult* result, RaycastHit* hit);
SOREN_EXPORT String* collision_result_to_string(CollisionResult* result, String* str);

SOREN_EXPORT void raycast_hit_reset(RaycastHit* hit);
SOREN_EXPORT void raycast_hit_to_collision_result(RaycastHit* hit, CollisionResult* result);
SOREN_EXPORT String* raycast_hit_to_string(RaycastHit* hit, String* str);

SOREN_EXPORT bool collision_circle_to_circle(CircleCollider* first, CircleCollider* second);
SOREN_EXPORT bool collision_circle_to_circle_ext(CircleCollider* first, CircleCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_circle_to_radius(CircleCollider* circle, Vector position, float radius);
SOREN_EXPORT bool collision_circle_to_radius_ext(CircleCollider* circle, Vector position, float radius, CollisionResult* out_result);

SOREN_EXPORT bool collision_radius_to_radius(Vector first_position, float first_radius, Vector second_position, float second_radius);
SOREN_EXPORT bool collision_radius_to_radius_ext(Vector first_position, float first_radius, Vector second_position, float second_radius, CollisionResult* out_result);

SOREN_EXPORT bool collision_circle_to_box(CircleCollider* first, BoxCollider* second);
SOREN_EXPORT bool collision_circle_to_box_ext(CircleCollider* first, BoxCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_circle_to_rect(CircleCollider* first, RectF second);
SOREN_EXPORT bool collision_circle_to_rect_ext(CircleCollider* first, RectF second, CollisionResult* out_result);

SOREN_EXPORT bool collision_radius_to_rect(Vector position, float radius, RectF rect);
SOREN_EXPORT bool collision_radius_to_rect_ext(Vector position, float radius, RectF rect, CollisionResult* out_result);

SOREN_EXPORT bool collision_circle_to_polygon(CircleCollider* first, PolygonCollider* second);
SOREN_EXPORT bool collision_circle_to_polygon_ext(CircleCollider* first, PolygonCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_radius_to_polygon(Vector position, float radius, PolygonCollider* second);
SOREN_EXPORT bool collision_radius_to_polygon_ext(Vector position, float radius, PolygonCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_radius_to_shape(Vector position, float radius, Vector* points, int points_count, Vector shape_position);
SOREN_EXPORT bool collision_radius_to_shape_ext(Vector position, float radius, Vector* points, int points_count, Vector shape_position, CollisionResult* out_result);

SOREN_EXPORT bool collision_polygon_to_polygon(PolygonCollider* first, PolygonCollider* second);
SOREN_EXPORT bool collision_polygon_to_polygon_ext(PolygonCollider* first, PolygonCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_polygon_to_shape(PolygonCollider* first, Vector* points, Vector* edge_normals, int points_count, Vector shape_position);
SOREN_EXPORT bool collision_polygon_to_shape_ext(PolygonCollider* first, Vector* points, Vector* edge_normals, int points_count, Vector shape_position, CollisionResult* out_result);

SOREN_EXPORT bool collision_shape_to_shape(
    Vector* first_points,
    Vector* first_edge_normals,
    int first_points_count,
    Vector first_position,
    Vector* second_points,
    Vector* second_edge_normals,
    int second_points_count,
    Vector second_position);

SOREN_EXPORT bool collision_shape_to_shape_ext(
    Vector* first_points,
    Vector* first_edge_normals,
    int first_points_count,
    Vector first_position,
    Vector* second_points,
    Vector* second_edge_normals,
    int second_points_count,
    Vector second_position,
    CollisionResult* out_result);

SOREN_EXPORT bool collision_box_to_box(BoxCollider* first, BoxCollider* second);
SOREN_EXPORT bool collision_box_to_box_ext(BoxCollider* first, BoxCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_box_to_rect(BoxCollider* first, RectF second);
SOREN_EXPORT bool collision_box_to_rect_ext(BoxCollider* first, RectF second, CollisionResult* out_result);

SOREN_EXPORT bool collision_rect_to_rect(RectF first, RectF second);
SOREN_EXPORT bool collision_rect_to_rect_ext(RectF first, RectF second, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_circle(Vector point, CircleCollider* circle);
SOREN_EXPORT bool collision_point_to_circle_ext(Vector point, CircleCollider* circle, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_radius(Vector point, Vector circle_pos, float radius);
SOREN_EXPORT bool collision_point_to_radius_ext(Vector point, Vector circle_pos, float radius, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_box(Vector point, BoxCollider* box);
SOREN_EXPORT bool collision_point_to_box_ext(Vector point, BoxCollider* box, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_rect(Vector point, RectF box);
SOREN_EXPORT bool collision_point_to_rect_ext(Vector point, RectF box, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_poly(Vector point, PolygonCollider* poly);
SOREN_EXPORT bool collision_point_to_poly_ext(Vector point, PolygonCollider* poly, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_shape(Vector point, Vector* points, int points_count, Vector shape_position);
SOREN_EXPORT bool collision_point_to_shape_ext(Vector point, Vector* points, int points_count, Vector shape_position, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_line(Vector point, LineCollider* line);
SOREN_EXPORT bool collision_point_to_line_ext(Vector point, LineCollider* line, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_segment(Vector point, Vector line_start, Vector line_end);
SOREN_EXPORT bool collision_point_to_segment_ext(Vector point, Vector line_start, Vector line_end, CollisionResult* out_result);

SOREN_EXPORT bool collision_point_to_point(Vector first, Vector second);
SOREN_EXPORT bool collision_point_to_point_ext(Vector first, Vector second, CollisionResult* out_result);

SOREN_EXPORT bool collision_line_to_poly(LineCollider* line, PolygonCollider* polygon);
SOREN_EXPORT bool collision_line_to_poly_ext(LineCollider* line, PolygonCollider* polygon, RaycastHit* out_result);

SOREN_EXPORT bool collision_segment_to_poly(Vector start, Vector end, PolygonCollider* polygon);
SOREN_EXPORT bool collision_segment_to_poly_ext(Vector start, Vector end, PolygonCollider* polygon, RaycastHit* out_result);

SOREN_EXPORT bool collision_segment_to_shape(Vector start, Vector end, Vector* points, int points_count, Vector shape_position);
SOREN_EXPORT bool collision_segment_to_shape_ext(Vector start, Vector end, Vector* points, int points_count, Vector shape_position, RaycastHit* out_result);

SOREN_EXPORT bool collision_line_to_circle(LineCollider* line, CircleCollider* circle);
SOREN_EXPORT bool collision_line_to_circle_ext(LineCollider* line, CircleCollider* circle, RaycastHit* out_result);

SOREN_EXPORT bool collision_segment_to_circle(Vector start, Vector end, CircleCollider* circle);
SOREN_EXPORT bool collision_segment_to_circle_ext(Vector start, Vector end, CircleCollider* circle, RaycastHit* out_result);

SOREN_EXPORT bool collision_line_to_radius(LineCollider* line, Vector position, float radius);
SOREN_EXPORT bool collision_line_to_radius_ext(LineCollider* line, Vector position, float radius, RaycastHit* out_result);

SOREN_EXPORT bool collision_segment_to_radius(Vector start, Vector end, Vector position, float radius);
SOREN_EXPORT bool collision_segment_to_radius_ext(Vector start, Vector end, Vector position, float radius, RaycastHit* out_result);

SOREN_EXPORT bool collision_line_to_line(LineCollider* first, LineCollider* second);
SOREN_EXPORT bool collision_line_to_line_ext(LineCollider* first, LineCollider* second, CollisionResult* out_result);

SOREN_EXPORT bool collision_line_to_segment(LineCollider* line, Vector start, Vector end);
SOREN_EXPORT bool collision_line_to_segment_ext(LineCollider* line, Vector start, Vector end, CollisionResult* out_result);

SOREN_EXPORT bool collision_segment_to_segment(Vector first_start, Vector first_end, Vector second_start, Vector second_end);
SOREN_EXPORT bool collision_segment_to_segment_ext(Vector first_start, Vector first_end, Vector second_start, Vector second_end, CollisionResult* out_result);

#endif