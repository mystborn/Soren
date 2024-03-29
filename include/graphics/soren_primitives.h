#ifndef SOREN_GRAPHICS_SOREN_PRIMITIVES_H
#define SOREN_GRAPHICS_SOREN_PRIMITIVES_H

#include "../soren_std.h"
#include "../soren_math.h"

#define CIRCLE_SEGMENT_AUTO -1

SOREN_EXPORT void draw_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color);

SOREN_EXPORT void draw_filled_convex_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_filled_convex_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color);

SOREN_EXPORT void draw_filled_concave_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_filled_concave_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color);

SOREN_EXPORT void draw_rect_rgba(SDL_Renderer* renderer, RectF Rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_rect_color(SDL_Renderer* renderer, RectF Rect, SDL_FColor color);

SOREN_EXPORT void draw_filled_rect_rgba(SDL_Renderer* renderer, RectF Rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_filled_rect_color(SDL_Renderer* renderer, RectF Rect, SDL_FColor color);

SOREN_EXPORT void draw_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_circle_color(SDL_Renderer* renderer, Vector position, float radius, float thickness, int segments, SDL_FColor color);

SOREN_EXPORT void draw_filled_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_filled_circle_color(SDL_Renderer* renderer, Vector position, float radius, int segments, SDL_FColor color);

SOREN_EXPORT void draw_line_rgba(SDL_Renderer* renderer, Vector start, Vector end, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_line_color(SDL_Renderer* renderer, Vector start, Vector end, float thickness,  SDL_FColor color);

SOREN_EXPORT void draw_point_rgba(SDL_Renderer* renderer, Vector point, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_point_color(SDL_Renderer* renderer, Vector point, SDL_FColor color);

SOREN_EXPORT void draw_arc_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_arc_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, SDL_FColor color);

SOREN_EXPORT void draw_pie_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_pie_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, SDL_FColor color);

SOREN_EXPORT void draw_filled_pie_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SOREN_EXPORT void draw_filled_pie_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, SDL_FColor color);

#endif