#include <soren_generics.h>

LIST_DEFINE_C(IntList, int_list, int)
LIST_DEFINE_C(VectorList, vector_list, Vector)
LIST_DEFINE_C(PointList, point_list, Point)
LIST_DEFINE_C(RectFList, rectf_list, RectF)
LIST_DEFINE_C(ScancodeList, scancode_list, SDL_Scancode)
LIST_DEFINE_C(GamepadButtonList, gamepad_button_list, uint32_t)
LIST_DEFINE_C(GamepadActionList, gamepad_action_list, GamepadAction)
LIST_DEFINE_C(MouseButtonList, mouse_button_list, MouseButtons)
LIST_DEFINE_C(TextureList, texture_list, SDL_Texture*)

MAP_DEFINE_C(ColorMap, color_map, char*, SDL_FColor, gds_fnv32, strcmp)