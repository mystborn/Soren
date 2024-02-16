#ifndef SOREN_COLLISIONS_SHARED_H
#define SOREN_COLLISIONS_SHARED_H

#include <collisions/soren_colliders.h>

void polygon_collider_free_resources(PolygonCollider* polygon);
void point_collider_free_resources(PointCollider* point);
void collider_assert_scale(float scale);
BoxCollider* collision_shared_box_collider_init(RectF bounds);

#endif