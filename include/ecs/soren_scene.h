#include "../soren_std.h"
#include "../soren_generics.h"
#include "../collisions/soren_spatial_hash.h"
#include <ecs.h>

typedef struct Scene {
    EcsSequentialSystem* update;
    EcsSequentialSystem* draw;
    EcsSequentialSystem* gui;
    SpatialHash* hash;
    CameraList* cameras;
    Camera* gui_camera;
    SDL_Renderer* renderer;
    EcsWorld world;
} Scene;

typedef enum SceneDestroyParams {
    SCENE_DESTROY_NONE,
    SCENE_DESTROY_SYSTEMS = 1,
    SCENE_DESTROY_CAMERAS = 2,
    SCENE_DESTROY_GUI_CAMERA = 4,
    SCENE_DESTROY_WORLD = 8,
    SCENE_DESTROY_SPATIAL_HASH = 16,
    SCENE_DESTROY_ALL = 31,
    SCENE_DESTROY_ALL_BUT_CAMERAS = 25,
    SCENE_DESTROY_ALL_BUT_GUI_CAMERA = 27
} SceneDestroyParams;

#define SOREN_ECS_ASSERT(result) soren_assert(result == ECS_RESULT_SUCCESS)

SOREN_EXPORT Scene* scene_create(EcsWorld world, SDL_Renderer* renderer, SpatialHash* hash, CameraList* cameras, Camera* gui_camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui);
SOREN_EXPORT void scene_init(Scene* scene, EcsWorld world, SDL_Renderer* renderer, SpatialHash* hash, CameraList* cameras, Camera* gui_camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui);
SOREN_EXPORT void scene_free_resources(Scene* scene, SceneDestroyParams params);
SOREN_EXPORT void scene_free(Scene* scene, SceneDestroyParams params);

SOREN_EXPORT void scene_update(Scene* scene, float delta);
SOREN_EXPORT void scene_draw(Scene* scene, float delta);

SOREN_EXPORT void scene_push(Scene* scene);
SOREN_EXPORT void scene_change(Scene* scene, SceneDestroyParams params);
SOREN_EXPORT Scene* scene_pop(SceneDestroyParams params);
SOREN_EXPORT Scene* scene_current(void);