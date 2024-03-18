#include <ecs/soren_scene.h>
#include <generic_list.h>
#include <generic_iterators/list_iterator.h>
#include <graphics/soren_graphics.h>

LIST_DEFINE_H(SorenSceneList, soren_scene_list, Scene*)
LIST_DEFINE_C(SorenSceneList, soren_scene_list, Scene*)

static SorenSceneList scenes;

SOREN_EXPORT Scene* scene_create(EcsWorld world, SDL_Renderer* renderer, SpatialHash* hash, CameraList* cameras, Camera* gui_camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui) {
    Scene* scene = soren_malloc(sizeof(*scene));
    scene_init(scene, world, renderer, hash, cameras, gui_camera, update, draw, gui);
    return scene;
}

SOREN_EXPORT void scene_init(Scene* scene, EcsWorld world, SDL_Renderer* renderer, SpatialHash* hash, CameraList* cameras, Camera* gui_camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui) {
    scene->world = world;
    scene->renderer = renderer;
    scene->cameras = cameras;
    scene->gui_camera = gui_camera;
    scene->update = update;
    scene->draw = draw;
    scene->gui = gui;
    scene->hash = hash;
}

SOREN_EXPORT void scene_free_resources(Scene* scene, SceneDestroyParams params) {
    if (SOREN_ENUM_FLAG(params, SCENE_DESTROY_SYSTEMS)) {
        ecs_system_free_resources((EcsSystem*)scene->update);
        ecs_system_free_resources((EcsSystem*)scene->draw);
        ecs_system_free_resources((EcsSystem*)scene->gui);
        soren_free(scene->update);
        soren_free(scene->draw);
        soren_free(scene->gui);
    }

    if (SOREN_ENUM_FLAG(params, SCENE_DESTROY_GUI_CAMERA)) {
        camera_free(scene->gui_camera);
    }

    if (SOREN_ENUM_FLAG(params, SCENE_DESTROY_CAMERAS)) {
        Camera* camera;
        list_iter_start(scene->cameras, camera) {
            camera_free(camera);
        }
        list_iter_end
        camera_list_free(scene->cameras);
    }

    if (SOREN_ENUM_FLAG(params, SCENE_DESTROY_WORLD)) {
        EcsResult result = ecs_world_free(scene->world);
        SOREN_ECS_ASSERT(result);
    }

    if (scene->hash && SOREN_ENUM_FLAG(params, SCENE_DESTROY_SPATIAL_HASH)) {
        spatial_hash_free(scene->hash);
    }
}

SOREN_EXPORT void scene_free(Scene* scene, SceneDestroyParams params) {
    scene_free_resources(scene, params);
    soren_free(scene);
}

SOREN_EXPORT void scene_update(Scene* scene, float delta) {
    ecs_system_update((EcsSystem*)scene->update, delta);
}

SOREN_EXPORT void scene_draw(Scene* scene, float delta) {
    SDL_SetRenderTarget(scene->renderer, NULL);
    SDL_SetRenderDrawColorFloat(scene->renderer, COLOR_DECONSTRUCT(soren_background_color));
    SDL_RenderClear(scene->renderer);


    RectF viewport = RECTF_EMPTY;

    if (rectf_is_empty(viewport)) {
        int w;
        int h;

        SDL_Window* window = SDL_GetRenderWindow(scene->renderer);
        SDL_GetWindowSizeInPixels(window, &w, &h);

        viewport.x = 0;
        viewport.y = 0;
        viewport.w = (float)w;
        viewport.h = (float)h;
    }

    Rect clip = rectf_to_rect(viewport);

    if (scene->cameras && camera_list_count(scene->cameras)) {
        Camera* camera;
        list_iter_start(scene->cameras, camera) {
            graphics_set_camera(camera);
            SDL_SetRenderTarget(scene->renderer, camera->render_target);

            SDL_SetRenderDrawColorFloat(scene->renderer, COLOR_DECONSTRUCT(soren_background_color));
            SDL_RenderClear(scene->renderer);

            ecs_system_update((EcsSystem*)scene->draw, delta);
        }
        list_iter_end

        SDL_SetRenderTarget(scene->renderer, NULL);
        SDL_SetRenderClipRect(scene->renderer, &clip);

        list_iter_start(scene->cameras, camera) {
            RectF camera_viewport = camera->viewport;
            if (rectf_is_empty(camera_viewport)) {
                camera_viewport = viewport;
            } else {
                camera_viewport.x += viewport.x;
                camera_viewport.y += viewport.y;                
            }
            SDL_RenderTextureRotated(scene->renderer, camera->render_target, NULL, &camera_viewport, camera->viewport_rotation, NULL, SDL_FLIP_NONE);
        }
        list_iter_end

        SDL_SetRenderClipRect(scene->renderer, NULL);
    } else {
        SDL_SetRenderClipRect(scene->renderer, &clip);

        ecs_system_update((EcsSystem*)scene->draw, delta);

        SDL_SetRenderClipRect(scene->renderer, NULL);
    }

    if (scene->gui_camera) {
        SDL_SetRenderTarget(scene->renderer, scene->gui_camera->render_target);
        SDL_SetRenderDrawColorFloat(scene->renderer, COLOR_DECONSTRUCT(soren_gui_background_color));
        SDL_RenderClear(scene->renderer);
        
        ecs_system_update((EcsSystem*)scene->gui, delta);

        SDL_SetRenderTarget(scene->renderer, NULL);
        SDL_SetRenderClipRect(scene->renderer, &clip);


        RectF camera_viewport = scene->gui_camera->viewport;
        if (rectf_is_empty(camera_viewport)) {
            camera_viewport = viewport;
        } else {
            camera_viewport.x += viewport.x;
            camera_viewport.y += viewport.y;                
        }

        SDL_RenderTextureRotated(
            scene->renderer,
            scene->gui_camera->render_target,
            NULL,
            &camera_viewport,
            scene->gui_camera->viewport_rotation,
            NULL,
            SDL_FLIP_NONE);

        SDL_SetRenderClipRect(scene->renderer, NULL);
    } else {
        SDL_SetRenderClipRect(scene->renderer, &clip);
        ecs_system_update((EcsSystem*)scene->gui, delta);
        SDL_SetRenderClipRect(scene->renderer, NULL);
    }
}

SOREN_EXPORT void scene_push(Scene* scene) {
    soren_scene_list_add(&scenes, scene);
}

SOREN_EXPORT void scene_change(Scene* scene, SceneDestroyParams params) {
    while (soren_scene_list_count(&scenes) > 0) {
        scene_pop(params);
    }

    scene_push(scene);
}

SOREN_EXPORT Scene* scene_pop(SceneDestroyParams params) {
    if (soren_scene_list_count(&scenes) == 0) {
        return NULL;
    }

    Scene* scene = soren_scene_list_pop(&scenes);

    if (params != SCENE_DESTROY_NONE) {
        scene_free(scene, params);
    }

    return scene;
}

SOREN_EXPORT Scene* scene_current(void) {
    if (soren_scene_list_count(&scenes) == 0) {
        return NULL;
    }

    return soren_scene_list_peek(&scenes);
}