#ifndef SOREN_ECS_SOREN_WORLD_USE_COLLISIONS_H
#define SOREN_ECS_SOREN_WORLD_USE_COLLISIONS_H

#include "../soren_std.h"
#include "../collisions/soren_spatial_hash.h"

#include <ecs.h>

SOREN_EXPORT void soren_world_use_collisions(EcsWorld world, EcsComponentManager* collider_component, SpatialHash* hash);

#endif