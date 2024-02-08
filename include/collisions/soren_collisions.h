#ifndef SOREN_COLLISIONS_SOREN_COLLISIONS_H
#define SOREN_COLLISIONS_SOREN_COLLISIONS_H

#include "soren_colliders.h"

typedef struct CollisionResult {
    Vector normal;
    Vector minimum_translation_vector;
    Vector point;
} CollisionResult;

typedef struct RaycastHit {
    Collider* shape;
    float fraction;
    float distance;
    Vector point;
    Vector normal;
    Vector centroid;
} RaycastHit;

void collision_result_remove_horizonal_translation(CollisionResult* result, Vector delta);
void collision_result_invert(CollisionResult* result);
String* collision_result_to_string(CollisionResult* result, String* str);

void raycast_hit_reset(RaycastHit* hit);
String* raycast_hit_to_string(RaycastHit* hit, String* str);

bool collision_circle_to_circle(CircleCollider* first, CircleCollider* second);
bool collision_circle_to_circle_ext(CircleCollider* first, CircleCollider* second, CollisionResult* out_result);

bool collision_circle_to_box(CircleCollider* first, BoxCollider* second);
bool collision_circle_to_box_ext(CircleCollider* first, BoxCollider* second, CollisionResult* out_result);

bool collision_circle_to_rect(CircleCollider* first, RectF* second);
bool collision_circle_to_rect_ext(CircleCollider* first, RectF* second, CollisionResult* out_result);

bool collision_circle_to_polygon(CircleCollider* first, PolygonCollider* second);
bool collision_circle_to_polygon_ext(CircleCollider* first, PolygonCollider* second, CollisionResult* out_result);

bool collision_polygon_to_polygon(PolygonCollider* first, PolygonCollider* second);
bool collision_polygon_to_polygon_ext(PolygonCollider* first, PolygonCollider* second, CollisionResult* out_result);

bool collision_box_to_box(BoxCollider* first, BoxCollider* second);
bool collision_box_to_box_ext(BoxCollider* first, BoxCollider* second, CollisionResult* out_result);

bool collision_box_to_rect(BoxCollider* first, RectF second);
bool collision_box_to_rect_ext(BoxCollider* first, RectF second, CollisionResult* out_result);

bool collision_rect_to_rect(RectF first, RectF second);
bool collision_rect_to_rect_ext(RectF first, RectF second, CollisionResult* out_result);

bool collision_point_to_circle(Vector point, CircleCollider* circle);
bool collision_point_to_circle_ext(Vector point, CircleCollider* circle, CollisionResult* out_result);

bool collision_point_to_radius(Vector point, Vector circle_pos, float radius);
bool collision_point_to_radius_ext(Vector point, Vector circle_pos, float radius, CollisionResult* out_result);

bool collision_point_to_box(Vector point, BoxCollider* box);
bool collision_point_to_box(Vector point, BoxCollider* box, CollisionResult* out_result);

bool collision_point_to_rect(Vector point, RectF box);
bool collision_point_to_rect_ext(Vector point, RectF box, CollisionResult* out_result);

bool collision_point_to_poly(Vector point, PolygonCollider* poly);
bool collision_point_to_poly_ext(Vector point, PolygonCollider* poly, CollisionResult* out_result);

bool collision_point_to_line(Vector point, LineCollider* line);
bool collision_point_to_line_ext(Vector point, LineCollider* line, CollisionResult* out_result);

bool collision_point_to_segment(Vector point, Vector line_start, Vector line_end);
bool collision_point_to_segment_ext(Vector point, Vector line_start, Vector line_end, CollisionResult* out_result);

bool collision_point_to_point(Vector first, Vector second);
bool collision_point_to_point_ext(Vector first, Vector second, CollisionResult* out_result);

bool collision_line_to_poly(LineCollider* line, PolygonCollider* polygon);
bool collision_line_to_poly(LineCollider* line, PolygonCollider* polygon, RaycastHit* out_result);

bool collision_segment_to_poly(Vector start, Vector end, PolygonCollider* polygon);
bool collision_segment_to_poly_ext(Vector start, Vector end, PolygonCollider* polygon, RaycastHit* out_result);

bool collision_line_to_circle(LineCollider* line, CircleCollider* circle);
bool collision_line_to_circle_ext(LineCollider* line, CircleCollider* circle, RaycastHit* out_result);

bool collision_segment_to_circle(Vector start, Vector end, CircleCollider* circle);
bool collision_segment_to_circle_ext(Vector start, Vector end, CircleCollider* circle, RaycastHit* out_result);

bool collision_line_to_radius(LineCollider* line, Vector position, float radius);
bool collision_line_to_radius_ext(LineCollider* line, Vector position, float radius, RaycastHit* out_result);

bool collision_segment_to_radius(Vector start, Vector end, Vector position, float radius);
bool collision_segment_to_radius_ext(Vector start, Vector end, Vector position, float radius, RaycastHit* out_result);

bool collision_line_to_line(LineCollider* first, LineCollider* second);
bool collision_line_to_line_ext(LineCollider* first, LineCollider* second, CollisionResult* out_result);

bool collision_line_to_segment(LineCollider* line, Vector start, Vector end);
bool collision_line_to_segment_ext(LineCollider* line, Vector start, Vector end, CollisionResult* out_result);

bool collision_segment_to_segment(Vector first_start, Vector first_end, Vector second_start, Vector second_end);
bool collision_segment_to_segment_ext(Vector first_start, Vector first_end, Vector second_start, Vector second_end, CollisionResult* out_result);

#endif