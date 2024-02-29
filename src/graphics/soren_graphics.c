#include <graphics/soren_graphics.h>

static Camera* global_camera;

SOREN_EXPORT void graphics_set_camera(Camera* camera) {
    global_camera = camera;
}

SOREN_EXPORT Camera* graphics_get_camera(void) {
    return global_camera;
}