#include <graphics/soren_primitives.h>
#include <graphics/soren_graphics.h>

#include <soren_generics.h>
#include <generic_array.h>

static soren_thread_local Vector* points_cache = NULL;
static soren_thread_local int points_cache_capacity = 0;

static soren_thread_local SDL_Vertex* vertex_cache = NULL;
static soren_thread_local int vertex_cache_capacity = 0;

static soren_thread_local int* index_cache = NULL;
static soren_thread_local int index_cache_capacity = 0;

static Vector* fix_polygon_points(Vector* points, int points_count, int* out_points_count) {
    soren_assert(points_count > 0);
    int extra = vector_equals(points[0], points[points_count - 1]) ? 0 : 1;

    GDS_ARRAY_RESIZE(points_cache, points_cache_capacity, points_count + extra, sizeof(*points_cache));
    memcpy(points_cache, points, points_count * sizeof(*points_cache));
    if (extra == 1) {
        points_cache[points_count] = points[0];
    }
    *out_points_count = points_count + extra;

    return points_cache;
}

SOREN_EXPORT void draw_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_polygon_color(renderer, points, points_count, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color) {
    points = fix_polygon_points(points, points_count, &points_count);

    Matrix transform;
    if (graphics_using_camera(renderer, &transform)) {
        vector_transform_batch(points, points_count, points, &transform);
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    SDL_RenderLines(renderer, points, points_count);
}

// If the polygon is convex, simply choose the first vertex, and create a fan 
// from that point to each of the other vertices to construct the triangles to draw.
// Otherwise, use the ear clipping algorithm to construct the list of triangles to draw.

// https://en.wikipedia.org/wiki/Fan_triangulation
//
// Implementation of ear clipping tutorial:
// https://www.youtube.com/watch?v=hTJFcHutls8

static void generate_polygon_vertices(
    SDL_Renderer* renderer,
    Vector* points, 
    int points_count, 
    SDL_FColor color,
    SDL_Vertex** out_vertices,
    int* out_vertex_count,
    int** out_indices,
    int* out_index_count)
{
    if (points && vector_equals(points[0], points[points_count - 1])) {
        points_count -= 1;
    }

    Matrix transform;
    graphics_using_camera(renderer, &transform);

    GDS_ARRAY_RESIZE(vertex_cache, vertex_cache_capacity, points_count, sizeof(*vertex_cache));
    *out_vertices = vertex_cache;
    *out_vertex_count = points_count;

    for (int i = 0; i < points_count; i++) {
        vertex_cache[i].color = color;
        if (points) {
            vertex_cache[i].position = vector_transform(points[i], &transform);
        }
    }

    int index_count = (points_count - 2) * 3;
    GDS_ARRAY_RESIZE(index_cache, index_cache_capacity, index_count, sizeof(*index_cache));
    *out_indices = index_cache;
    *out_index_count = index_count;
}

SOREN_EXPORT void draw_filled_convex_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_convex_polygon_color(renderer, points, points_count, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_filled_convex_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color) {
    int vertex_count;
    SDL_Vertex* vertex_array;
    int index_count;
    int* index_array;

    generate_polygon_vertices(
        renderer,
        points,
        points_count,
        color,
        &vertex_array,
        &vertex_count,
        &index_array,
        &index_count);

    int index = 0;
    for (int i = 2; i < vertex_count; i++) {
        index_array[index++] = 0;
        index_array[index++] = i - 1;
        index_array[index++] = i;
    }

    SDL_RenderGeometry(renderer, NULL, vertex_array, vertex_count, index_array, index_count);
}

SOREN_EXPORT void draw_filled_concave_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_concave_polygon_color(renderer, points, points_count, COLOR_CONSTRUCT(r, g, b, a));
}

static bool point_in_triangle(Vector p, Vector a, Vector b, Vector c) {
    Vector ab = vector_subtract(b, a);
    Vector bc = vector_subtract(c, b);
    Vector ca = vector_subtract(a, c);

    Vector ap = vector_subtract(p, a);
    Vector bp = vector_subtract(p, b);
    Vector cp = vector_subtract(p, a);

    float dot1 = vector_cross(ab, ap);
    float dot2 = vector_cross(bc, bp);
    float dot3 = vector_cross(ca, cp);

    if (dot1 <= 0 || dot2 <= 0 || dot3 <= 0) {
        return false;
    }

    return true;
}

SOREN_EXPORT void draw_filled_concave_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_FColor color) {
    int vertex_count;
    SDL_Vertex* vertex_array;
    int index_count;
    int* index_array;
    IntList remaining_vertices;
    int current_index = 0;

    generate_polygon_vertices(
        renderer,
        points,
        points_count,
        color,
        &vertex_array,
        &vertex_count,
        &index_array,
        &index_count);

    int_list_init_capacity(&remaining_vertices, vertex_count);
    for (int i = 0; i < vertex_count; i++) {
        int_list_add(&remaining_vertices, i);
    }

    // Perform ear clipping algorithm to populate the index_array.
    while (int_list_count(&remaining_vertices) > 3) {
        int count = int_list_count(&remaining_vertices);
        for (int i = 0; i < count; i++) {
            int a = int_list_get(&remaining_vertices, i);
            int b = int_list_get(&remaining_vertices, i - 1 < 0 ? count - 1 : i - 1);
            int c = int_list_get(&remaining_vertices, i + 1 >= count ? 0 : i + 1);

            Vector curr_vertex = points[a];
            Vector prev_vertex = points[b];
            Vector next_vertex = points[c];

            Vector curr_to_prev = vector_subtract(prev_vertex, curr_vertex);
            Vector curr_to_next = vector_subtract(next_vertex, curr_vertex);

            // printf(" curr_to_prev { %d, %d }, curr_to_next { %d, %d }\n", curr_to_prev.x, )

            // reflex angle, continue to next ear.
            if (vector_cross(curr_to_prev, curr_to_next) > 0) {
                continue;
            }

            // Check if any vertices are inside of the ear.
            bool is_ear = true;
            for (int j = 0; j < vertex_count; j++) {
                if (j == a || j == b || j == c) {
                    continue;
                }

                Vector p = points[j];

                if (point_in_triangle(p, prev_vertex, curr_vertex, next_vertex)) {
                    is_ear = false;
                    break;
                }
            }

            if (is_ear) {
                index_array[current_index++] = b;
                index_array[current_index++] = a;
                index_array[current_index++] = c;

                int_list_remove(&remaining_vertices, i);
                break;
            }
        }
    }

    index_array[current_index++] = int_list_get(&remaining_vertices, 0);
    index_array[current_index++] = int_list_get(&remaining_vertices, 1);
    index_array[current_index++] = int_list_get(&remaining_vertices, 2);

    SDL_RenderGeometry(renderer, NULL, vertex_array, vertex_count, index_array, index_count);

    SDL_SetRenderDrawColorFloat(renderer, 255, 0, 0, 255);

    int_list_free_resources(&remaining_vertices);
}

SOREN_EXPORT void draw_rect_rgba(SDL_Renderer* renderer, RectF rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_rect_color(renderer, rect, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_rect_color(SDL_Renderer* renderer, RectF rect, SDL_FColor color) {
    if (graphics_using_camera(renderer, NULL)) {
        Camera* camera = graphics_get_camera();
        if (camera_rotation(camera) == 0) {
            rect.x -= camera->bounds.x;
            rect.y -= camera->bounds.y;
        } else {
            Vector points[4];
            rectf_points(rect, points);
            draw_polygon_color(renderer, points, 4, color);
            return;
        }
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    SDL_RenderRect(renderer, &rect);
}

SOREN_EXPORT void draw_filled_rect_rgba(SDL_Renderer* renderer, RectF rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_rect_color(renderer, rect, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_filled_rect_color(SDL_Renderer* renderer, RectF rect, SDL_FColor color) {
    if (graphics_using_camera(renderer, NULL)) {
        Camera* camera = graphics_get_camera();
        if (camera_rotation(camera) == 0) {
            rect.x -= camera->bounds.x;
            rect.y -= camera->bounds.y;
        } else {
            Vector points[4];
            rectf_points(rect, points);
            draw_filled_convex_polygon_color(renderer, points, 4, color);
            return;
        }
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    SDL_RenderFillRect(renderer, &rect);
}

static inline int compute_segment_count(float radius) {
    if (radius <= 16) {
        return 12;
    } else if (radius <= 32) {
        return 16;
    } else if (radius <= 64) {
        return 24;
    } else {
        return 32;
    }
}

static inline Vector arc_step(Vector position, float tangential_factor, float radial_factor) {
    float tx = -position.y;
    float ty = position.x;

    position.x += tx * tangential_factor;
    position.y += ty * tangential_factor;

    position.x *= radial_factor;
    position.y *= radial_factor;

    return position;
}

static void arc_add_line(SDL_Vertex* vertices, int* indices, int vertex_position, int index_position, Vector start, Vector end, float half_width, SDL_FColor color) {
    Vector perp = vector_normalize(vector_perpendicular(start, end));
    Vector top = vector_multiply_scalar(perp, half_width);
    Vector bottom = vector_negate(top);

    vertices[vertex_position].color = color;
    vertices[vertex_position].position = vector_add(start, top);

    vertices[vertex_position + 1].color = color;
    vertices[vertex_position + 1].position = vector_add(end, top);

    vertices[vertex_position + 2].color = color;
    vertices[vertex_position + 2].position = vector_add(end, bottom);

    vertices[vertex_position + 3].color = color;
    vertices[vertex_position + 3].position = vector_add(start, bottom);

    indices[index_position++] = vertex_position;
    indices[index_position++] = vertex_position + 1;
    indices[index_position++] = vertex_position + 2;
    indices[index_position++] = vertex_position;
    indices[index_position++] = vertex_position + 2;
    indices[index_position++] = vertex_position + 3;
}

static void draw_arc_outline_parts(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, bool include_from_center, float thickness, SDL_FColor color) {
    soren_assert(thickness > 0);
    soren_assert(segments > 2);

    Matrix transform;
    graphics_using_camera(renderer, &transform);

    float theta = (end_angle - start_angle) / segments;
    float tangential_factor = tanf(theta);
    float radial_factor = cosf(theta);

    Vector radial_position = vector_create(
        radius * cos(start_angle),
        radius * sin(start_angle)
    );

    if (thickness == 1) {
        int point_count = segments + 1 + (include_from_center ? 2 : 0);
        int point_index = 0;
        GDS_ARRAY_RESIZE(points_cache, points_cache_capacity, point_count, sizeof(*vertex_cache));
        if (include_from_center) {
            points_cache[point_index++] = vector_transform(position, &transform);
        }

        for (int i = 0; i <= segments; i++) {
            points_cache[point_index++] = vector_transform(vector_add(position, radial_position), &transform);
            Vector next_position = arc_step(radial_position, tangential_factor, radial_factor);

            radial_position = next_position;
        }

        if (include_from_center) {
            points_cache[point_index++] = vector_transform(position, &transform);
        }

        SDL_RenderLines(renderer, points_cache, point_count);
    } else {
        // TODO: Optimize this algorithm. Should be possible to use less vertices
        //       by drawing from the previous line end vertices to the new line
        //       vertices. This should approximately halve the total vertices,
        //       and give a smoother overall circle.

        // Each segment requires 4 vertices and 6 indices.
        float half_thickness = thickness / 2;
        int vertex_count = (segments + (include_from_center ? 2 : 0)) * 4;
        int index_count = (segments + (include_from_center ? 2 : 0)) * 6;

        GDS_ARRAY_RESIZE(vertex_cache, vertex_cache_capacity, vertex_count, sizeof(*vertex_cache));
        GDS_ARRAY_RESIZE(index_cache, index_cache_capacity, index_count, sizeof(*index_cache));

        int index_position = 0;
        int vertex_position = 0;

        if (include_from_center) {
            arc_add_line(
                vertex_cache, 
                index_cache, 
                vertex_position, 
                index_position, 
                vector_transform(position, &transform), 
                vector_transform(vector_add(position, radial_position), &transform), 
                half_thickness,
                color);

            vertex_position += 4;
            index_position += 6;
        }

        for (int i = 0; i < segments; i++) {
            Vector next_position = arc_step(radial_position, tangential_factor, radial_factor);

            arc_add_line(
                vertex_cache, 
                index_cache, 
                vertex_position, 
                index_position, 
                vector_transform(vector_add(position, radial_position), &transform), 
                vector_transform(vector_add(position, next_position), &transform), 
                half_thickness, 
                color);

            vertex_position += 4;
            index_position += 6;

            radial_position = next_position;
        }

        if (include_from_center) {
            arc_add_line(
                vertex_cache, 
                index_cache, 
                vertex_position, 
                index_position, 
                vector_transform(vector_add(position, radial_position), &transform), 
                vector_transform(position, &transform), 
                half_thickness, 
                color);
        }

        SDL_RenderGeometry(renderer, NULL, vertex_cache, vertex_count, index_cache, index_count);
    }
}

static void draw_arc_filled_parts(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, SDL_FColor color) {
    Matrix transform;
    graphics_using_camera(renderer, &transform);

    float theta = (end_angle - start_angle) / segments;
    float tangential_factor = tanf(theta);
    float radial_factor = cosf(theta);

    Vector radial_position = vector_create(
        radius * cos(start_angle),
        radius * sin(start_angle)
    );

    int vertex_count = (segments + 2);
    int index_count = segments * 3;

    GDS_ARRAY_RESIZE(vertex_cache, vertex_cache_capacity, vertex_count, sizeof(*vertex_cache));
    GDS_ARRAY_RESIZE(index_cache, index_cache_capacity, index_count, sizeof(*index_cache));

    int index_position = 0;
    int vertex_position = 0;

    vertex_cache[vertex_position].color = color;
    vertex_cache[vertex_position++].position = vector_transform(position, &transform);

    vertex_cache[vertex_position].color = color;
    vertex_cache[vertex_position++].position = vector_transform(vector_add(position, radial_position), &transform);

    for (int i = 0; i < segments; i++) {
        Vector next_position = arc_step(radial_position, tangential_factor, radial_factor);

        vertex_cache[vertex_position].color = color;
        vertex_cache[vertex_position].position = vector_transform(vector_add(position, next_position), &transform);

        index_cache[index_position++] = 0;
        index_cache[index_position++] = vertex_position - 1;
        index_cache[index_position++] = vertex_position;

        vertex_position++;

        radial_position = next_position;
    }

    SDL_RenderGeometry(renderer, NULL, vertex_cache, vertex_position, index_cache, index_position);
}

static void draw_circle_pixels(SDL_Renderer* renderer, Vector center, float x, float y) {
    // float x = 0;
    // float y = radius;
    // float d = 3 - 2 * radius;
    // do {
    //     draw_circle_pixels(renderer, position, x, y);

    //     x++;
    //     if (d > 0) {
    //         y -= 1;
    //         d = d + 4 * (x - y) + 10;
    //     } else {
    //         d = d + 4 * x + 6;
    //     }
    // } while (x <= y);

    Vector points[8] = {
        vector_create(center.x + x, center.y + y),
        vector_create(center.x - x, center.y + y),
        vector_create(center.x + x, center.y - y),
        vector_create(center.x - x, center.y - y),
        vector_create(center.x + y, center.y + x),
        vector_create(center.x - y, center.y + x),
        vector_create(center.x + y, center.y - x),
        vector_create(center.x - y, center.y - x)
    };

    SDL_RenderPoints(renderer, points, 8);
}

SOREN_EXPORT void draw_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_circle_color(renderer, position, radius, thickness, segments, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_circle_color(SDL_Renderer* renderer, Vector position, float radius, float thickness, int segments, SDL_FColor color) {
    if (segments <= 0) {
        segments = compute_segment_count(radius);
    }
    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    draw_arc_outline_parts(renderer, position, radius, 0, degrees_to_radians(360), segments, false, thickness, color);
}

SOREN_EXPORT void draw_filled_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_circle_color(renderer, position, radius, segments, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_filled_circle_color(SDL_Renderer* renderer, Vector position, float radius, int segments, SDL_FColor color) {
    if (segments <= 0) {
        segments = compute_segment_count(radius);
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    draw_arc_filled_parts(renderer, position, radius, 0, degrees_to_radians(360), segments, color);
}

SOREN_EXPORT void draw_arc_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_arc_color(renderer, position, radius, start_angle, end_angle, thickness, segments, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_arc_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, SDL_FColor color) {
    if (segments <= 0) {
        segments = compute_segment_count(radius);
    }
    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    draw_arc_outline_parts(renderer, position, radius, start_angle, end_angle, segments, false, thickness, color);
}

SOREN_EXPORT void draw_pie_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_pie_color(renderer, position, radius, start_angle, end_angle, thickness, segments, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_pie_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, int segments, SDL_FColor color) {
    if (segments <= 0) {
        segments = compute_segment_count(radius);
    }
    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    draw_arc_outline_parts(renderer, position, radius, start_angle, end_angle, segments, true, thickness, color);
}

SOREN_EXPORT void draw_filled_pie_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_pie_color(renderer, position, radius, start_angle, end_angle, segments, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_filled_pie_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, int segments, SDL_FColor color) {
    if (segments <= 0) {
        segments = compute_segment_count(radius);
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    draw_arc_filled_parts(renderer, position, radius, start_angle, end_angle, segments, color);
}

SOREN_EXPORT void draw_line_rgba(SDL_Renderer* renderer, Vector start, Vector end, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_line_color(renderer, start, end, thickness, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_line_color(SDL_Renderer* renderer, Vector start, Vector end, float thickness, SDL_FColor color) {
    if (thickness < 0) {
        return;
    }

    Matrix transform;
    if (graphics_using_camera(renderer, &transform)) {
        start = vector_transform(start, &transform);
        end = vector_transform(end, &transform);
    }

    if (thickness == 1) {
        SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
        SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
    } else {
        float radius = thickness / 2;
        Vector perp = vector_perpendicular(start, end);
        perp = vector_normalize(perp);

        Vector top = vector_multiply_scalar(perp, radius);
        Vector bottom = vector_negate(top);

        SDL_FColor fcolor = color;

        SDL_Vertex vertices[4] = {
            {
                .color = fcolor,
                .position = vector_add(start, top)
            },
            {
                .color = fcolor,
                .position = vector_add(end, top)
            }, 
            {
                .color = fcolor,
                .position = vector_add(end, bottom)
            },
            {
                .color = fcolor,
                .position = vector_add(start, bottom)
            }
        };

        int indices[6] = { 0, 1, 2, 0, 2, 3 };

        int result = SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    }
}

SOREN_EXPORT void draw_point_rgba(SDL_Renderer* renderer, Vector point, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_point_color(renderer, point, COLOR_CONSTRUCT(r, g, b, a));
}

SOREN_EXPORT void draw_point_color(SDL_Renderer* renderer, Vector point, SDL_FColor color) {
    Matrix transform;
    if (graphics_using_camera(renderer, &transform)) {
        point = vector_transform(point, &transform);
    }

    SDL_SetRenderDrawColorFloat(renderer, COLOR_DECONSTRUCT(color));
    SDL_RenderPoint(renderer, point.x, point.y);
}