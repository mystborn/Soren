#ifndef SOREN_GENERICS_H
#define SOREN_GENERICS_H

#include "soren_std.h"

#include "generic_list.h"
#include "generic_map.h"
#include "generic_set.h"

#include "soren_math.h"
#include "soren_input.h"

#include "collisions/soren_colliders.h"

#include "graphics/soren_camera.h"

LIST_DEFINE_H(IntList, int_list, int)
LIST_DEFINE_H(VectorList, vector_list, Vector)
LIST_DEFINE_H(PointList, point_list, Point)
LIST_DEFINE_H(RectFList, rectf_list, RectF)
LIST_DEFINE_H(ScancodeList, scancode_list, SDL_Scancode)
LIST_DEFINE_H(GamepadButtonList, gamepad_button_list, uint32_t)
LIST_DEFINE_H(GamepadActionList, gamepad_action_list, GamepadAction)
LIST_DEFINE_H(MouseButtonList, mouse_button_list, MouseButtons)
LIST_DEFINE_H(TextureList, texture_list, SDL_Texture*)
LIST_DEFINE_H(CameraList, camera_list, Camera*)

MAP_DEFINE_H(ColorMap, color_map, char*, SDL_FColor)

#endif