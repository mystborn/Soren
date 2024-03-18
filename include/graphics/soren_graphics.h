#ifndef SOREN_GRAPHICS_SOREN_GRAPHICS_H
#define SOREN_GRAPHICS_SOREN_GRAPHICS_H

#include "../soren_std.h"
#include "soren_camera.h"

#include <SDL3/SDL.h>

#define COLOR_DECONSTRUCT(color) color.r, color.g, color.b, color.a

#define COLOR_CONSTRUCT_BYTES(r, g, b, a) (SDL_FColor){ (r) / 255.f, (g) / 255.f, (b) / 255.f, (a) / 255.f }
#define COLOR_CONSUTRCT_FLOATS(r, g, b, a) (SDL_FColor){ (r), (g), (b), (a) }

#define COLOR_CONSTRUCT(r, g, b, a) \
    _Generic((r), \
        uint8_t: COLOR_CONSTRUCT_BYTES(r, g, b, a), \
        int: COLOR_CONSTRUCT_BYTES(r, g, b, a), \
        float: COLOR_CONSUTRCT_FLOATS(r, g, b, a))

typedef struct WindowState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Camera* camera;
    SDL_FColor background_color;
    SDL_FColor gui_background_color;
    RectF viewport;
} WindowState;

SOREN_EXPORT void graphics_set_camera(Camera* camera);
SOREN_EXPORT Camera* graphics_get_camera(void);

static inline bool graphics_using_camera(SDL_Renderer* renderer, Matrix* out_view_matrix) {
    Camera* camera = graphics_get_camera();
    bool result = camera
        && camera->renderer == renderer
        && camera->render_target == SDL_GetRenderTarget(renderer);
    if (out_view_matrix) {
        if (result) {
            *out_view_matrix = camera_view_matrix(camera);
        } else {
            *out_view_matrix = MATRIX_IDENTITY;
        }
    }

    return result;
}

SOREN_EXPORT SDL_FColor soren_background_color;
SOREN_EXPORT SDL_FColor soren_gui_background_color;

struct SorenColors {
    SDL_FColor alice_blue;
    SDL_FColor antique_white;
    SDL_FColor aqua;
    SDL_FColor aquamarine;
    SDL_FColor azure;
    SDL_FColor beige;
    SDL_FColor bisque;
    SDL_FColor black;
    SDL_FColor blanched_almond;
    SDL_FColor blue;
    SDL_FColor blue_violet;
    SDL_FColor brown;
    SDL_FColor burly_wood;
    SDL_FColor cadet_blue;
    SDL_FColor chartreuse;
    SDL_FColor chocolate;
    SDL_FColor coral;
    SDL_FColor cornflower_blue;
    SDL_FColor cornsilk;
    SDL_FColor crimson;
    SDL_FColor cyan;
    SDL_FColor dark_blue;
    SDL_FColor dark_cyan;
    SDL_FColor dark_goldenrod;
    SDL_FColor dark_gray;
    SDL_FColor dark_green;
    SDL_FColor dark_grey;
    SDL_FColor dark_khaki;
    SDL_FColor dark_magenta;
    SDL_FColor dark_olive_green;
    SDL_FColor dark_orange;
    SDL_FColor dark_orchid;
    SDL_FColor dark_red;
    SDL_FColor dark_salmon;
    SDL_FColor dark_sea_green;
    SDL_FColor dark_slate_blue;
    SDL_FColor dark_slate_gray;
    SDL_FColor dark_slate_grey;
    SDL_FColor dark_turquoise;
    SDL_FColor dark_violet;
    SDL_FColor deep_pink;
    SDL_FColor deep_sky_blue;
    SDL_FColor dim_gray;
    SDL_FColor dim_grey;
    SDL_FColor dodger_blue;
    SDL_FColor fire_brick;
    SDL_FColor floral_white;
    SDL_FColor forest_green;
    SDL_FColor fuchsia;
    SDL_FColor gainsboro;
    SDL_FColor ghost_white;
    SDL_FColor gold;
    SDL_FColor goldenrod;
    SDL_FColor gray;
    SDL_FColor green;
    SDL_FColor green_yellow;
    SDL_FColor grey;
    SDL_FColor honeydew;
    SDL_FColor hot_pink;
    SDL_FColor indian_red;
    SDL_FColor indigo;
    SDL_FColor ivory;
    SDL_FColor khaki;
    SDL_FColor lavender;
    SDL_FColor lavender_blush;
    SDL_FColor lawn_green;
    SDL_FColor lemon_chiffon;
    SDL_FColor light_blue;
    SDL_FColor light_coral;
    SDL_FColor light_cyan;
    SDL_FColor light_goldenrod_yellow;
    SDL_FColor light_gray;
    SDL_FColor light_green;
    SDL_FColor light_grey;
    SDL_FColor light_pink;
    SDL_FColor light_salmon;
    SDL_FColor light_sea_green;
    SDL_FColor light_sky_blue;
    SDL_FColor light_slate_gray;
    SDL_FColor light_steel_blue;
    SDL_FColor light_yellow;
    SDL_FColor lime;
    SDL_FColor lime_green;
    SDL_FColor linen;
    SDL_FColor magenta;
    SDL_FColor maroon;
    SDL_FColor medium_aquamarine;
    SDL_FColor medium_blue;
    SDL_FColor medium_orchid;
    SDL_FColor medium_purple;
    SDL_FColor medium_seagreen;
    SDL_FColor medium_slateblue;
    SDL_FColor medium_springgreen;
    SDL_FColor medium_turquoise;
    SDL_FColor medium_violetred;
    SDL_FColor midnight_blue;
    SDL_FColor mint_cream;
    SDL_FColor misty_rose;
    SDL_FColor moccasin;
    SDL_FColor navajo_white;
    SDL_FColor navy;
    SDL_FColor old_lace;
    SDL_FColor olive;
    SDL_FColor olive_drab;
    SDL_FColor orange;
    SDL_FColor orange_red;
    SDL_FColor orchid;
    SDL_FColor pale_goldenrod;
    SDL_FColor pale_green;
    SDL_FColor pale_turquoise;
    SDL_FColor pale_violetred;
    SDL_FColor papaya_whip;
    SDL_FColor peach_puff;
    SDL_FColor peru;
    SDL_FColor pink;
    SDL_FColor plum;
    SDL_FColor powder_blue;
    SDL_FColor purple;
    SDL_FColor red;
    SDL_FColor rosy_brown;
    SDL_FColor royal_blue;
    SDL_FColor saddle_brown;
    SDL_FColor salmon;
    SDL_FColor sandy_brown;
    SDL_FColor sea_green;
    SDL_FColor seashell;
    SDL_FColor sienna;
    SDL_FColor silver;
    SDL_FColor sky_blue;
    SDL_FColor slate_blue;
    SDL_FColor slate_gray;
    SDL_FColor snow;
    SDL_FColor spring_green;
    SDL_FColor steel_blue;
    SDL_FColor tan;
    SDL_FColor teal;
    SDL_FColor thistle;
    SDL_FColor tomato;
    SDL_FColor turquoise;
    SDL_FColor violet;
    SDL_FColor wheat;
    SDL_FColor white;
    SDL_FColor white_smoke;
    SDL_FColor yellow;
    SDL_FColor yellow_green;
};

SOREN_EXPORT extern struct SorenColors soren_colors;

#endif