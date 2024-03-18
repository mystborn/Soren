#include <ecs/soren_world_use_collisions.h>

static EcsComponentManager* internal_collider_component;

static void collider_added(void* ctx, EcsComponentAddedMessage* message) {
    log_trace(soren_logger, "Collider added");

    SpatialHash* hash = ctx;
    spatial_hash_add(hash, (Collider*)message->component);
}

static void collider_removed(void* ctx, EcsComponentRemovedMessage* message) {
    log_trace(soren_logger, "Collider removed");

    SpatialHash* hash = ctx;
    spatial_hash_remove(hash, (Collider*)message->component);
}

static void entity_with_collider_disabled(void* ctx, EcsEntityDisabledMessage* message) {
    SpatialHash* hash = ctx;
    if (ecs_entity_has(message->entity, internal_collider_component)) {
        log_trace(soren_logger, "Collider removed because entity was disabled");

        Collider* collider;
        ecs_entity_get(message->entity, internal_collider_component, &collider);
        spatial_hash_remove(hash, collider);
    }
}

static void entity_with_collider_enabled(void* ctx, EcsEntityEnabledMessage* message) {
    SpatialHash* hash = ctx;
    if (ecs_entity_has(message->entity, internal_collider_component)) {
        log_trace(soren_logger, "Collider added because entity was enabled");

        Collider* collider;
        ecs_entity_get(message->entity, internal_collider_component, &collider);
        spatial_hash_add(hash, collider);
    }
}

static void entity_with_collider_disposed(void* ctx, EcsEntityDisposedMessage* message) {
    SpatialHash* hash = ctx;
    if (ecs_entity_has(message->entity, internal_collider_component)) {
        log_trace(soren_logger, "Collider removed because entity was disposed");

        Collider* collider;
        ecs_entity_get(message->entity, internal_collider_component, &collider);
        spatial_hash_remove(hash, collider);
    }
}

SOREN_EXPORT void soren_world_use_collisions(EcsWorld world, EcsComponentManager* collider_component, SpatialHash* hash) {
    internal_collider_component = collider_component;
    EcsEventManager* added = ecs_component_get_added_event(collider_component);
    ecs_event_subscribe(world, added, ecs_closure(hash, collider_added));
}