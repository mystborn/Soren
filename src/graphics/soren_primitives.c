#include <graphics/soren_primitives.h>
#include <soren_generics.h>

#define COLOR_DECONSTRUCT(color) color.r, color.g, color.b, color.a
#define PRIMITIVE_BLEND(a) a == 255 ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND

#define DRAW_CACHE_SIZE 32

static soren_thread_local Vector points_cache[DRAW_CACHE_SIZE];
static soren_thread_local SDL_Vertex vertex_cache[DRAW_CACHE_SIZE];
static soren_thread_local int index_cache[DRAW_CACHE_SIZE];

static Vector* fix_polygon_points(Vector* points, int points_count, int* out_points_count, bool* out_free_result) {
    soren_assert(points_count > 0);
    Vector* array;
    *out_free_result = false;
    if (vector_equals(points[0], points[points_count - 1])) {
        array = points;
        *out_points_count = points_count;
    } else if (points_count + 1 < DRAW_CACHE_SIZE) {
        array = points_cache;
        memcpy(array, points, points_count * sizeof(*array));
        array[points_count] = points[0];
        *out_points_count = points_count + 1;
    } else {
        array = soren_malloc((points_count + 1) * sizeof(*array));
        if (!array) {
            return NULL;
        }
        memcpy(array, points, points_count * sizeof(*array));
        array[points_count] = points[0];
        *out_free_result = true;
        *out_points_count = points_count + 1;
    }

    return array;
}

SOREN_EXPORT void draw_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    bool free_array = false;
    int final_points_count;
    Vector* array = fix_polygon_points(points, points_count, &final_points_count, &free_array);

    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderLines(renderer, array, final_points_count);

    if (free_array) {
        soren_free(array);
    }
}

SOREN_EXPORT void draw_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_Color color) {
    draw_polygon_rgba(renderer, points, points_count, COLOR_DECONSTRUCT(color));
}

// If the polygon is convex, simply choose the first vertex, and create a fan 
// from that point to each of the other vertices to construct the triangles to draw.
// Otherwise, use the ear clipping algorithm to construct the list of triangles to draw.

// https://en.wikipedia.org/wiki/Fan_triangulation
//
// Implementation of ear clipping tutorial:
// https://www.youtube.com/watch?v=hTJFcHutls8

static void generate_polygon_vertices(
    Vector* points, 
    int points_count, 
    SDL_Color color,
    SDL_Vertex** out_vertices,
    int* out_vertex_count,
    bool* free_vertex_array,
    int** out_indices,
    int* out_index_count,
    bool* free_index_array
    )
{
    if (vector_equals(points[0], points[points_count - 1])) {
        points_count -= 1;
    }

    *out_vertex_count = points_count;

    if (points_count <= DRAW_CACHE_SIZE) {
        *out_vertices = vertex_cache;
        *free_vertex_array = false;
    } else {
        *out_vertices = soren_malloc(points_count * sizeof(**out_vertices));
        *free_vertex_array = true;
    }

    for (int i = 0; i < points_count; i++) {
        (*out_vertices)[i].color = color;
        (*out_vertices)[i].position = points[i];
    }

    *out_index_count = (*out_vertex_count - 2) * 3;

    if (*out_index_count <= DRAW_CACHE_SIZE) {
        *out_indices = index_cache;
        *free_index_array = false;
    } else {
        *out_indices = soren_malloc(*out_index_count * sizeof(**out_indices));
        *free_index_array = true;
    }
}

SOREN_EXPORT void draw_filled_convex_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_convex_polygon_color(renderer, points, points_count, (SDL_Color){ r, g, b, a });
}

SOREN_EXPORT void draw_filled_convex_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_Color color) {
    int vertex_count;
    SDL_Vertex* vertex_array;
    bool free_vertex_array;
    int index_count;
    int* index_array;
    bool free_index_array;

    generate_polygon_vertices(
        points,
        points_count,
        color,
        &vertex_array,
        &vertex_count,
        &free_vertex_array,
        &index_array,
        &index_count,
        &free_index_array);


    int index = 0;
    for (int i = 2; i < vertex_count; i++) {
        index_array[index++] = 0;
        index_array[index++] = i - 1;
        index_array[index++] = i;
    }

    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(color.a));
    SDL_RenderGeometry(renderer, NULL, vertex_array, vertex_count, index_array, index_count);

    if (free_vertex_array) {
        soren_free(vertex_array);
    }

    if (free_index_array) {
        soren_free(index_array);
    }
}

SOREN_EXPORT void draw_filled_concave_polygon_rgba(SDL_Renderer* renderer, Vector* points, int points_count, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_concave_polygon_color(renderer, points, points_count, (SDL_Color){ r, g, b, a });
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

SOREN_EXPORT void draw_filled_concave_polygon_color(SDL_Renderer* renderer, Vector* points, int points_count, SDL_Color color) {
    int vertex_count;
    SDL_Vertex* vertex_array;
    bool free_vertex_array;
    int index_count;
    int* index_array;
    bool free_index_array;
    IntList remaining_vertices;
    int current_index = 0;

    generate_polygon_vertices(
        points,
        points_count,
        color,
        &vertex_array,
        &vertex_count,
        &free_vertex_array,
        &index_array,
        &index_count,
        &free_index_array);

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

    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(color.a));
    SDL_RenderGeometry(renderer, NULL, vertex_array, vertex_count, index_array, index_count);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    if (free_vertex_array) {
        soren_free(vertex_array);
    }

    if (free_index_array) {
        soren_free(index_array);
    }

    int_list_free_resources(&remaining_vertices);
}

SOREN_EXPORT void draw_rect_rgba(SDL_Renderer* renderer, RectF rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderRect(renderer, &rect);
}

SOREN_EXPORT void draw_rect_color(SDL_Renderer* renderer, RectF rect, SDL_Color color) {
    draw_rect_rgba(renderer, rect, COLOR_DECONSTRUCT(color));
}

SOREN_EXPORT void draw_filled_rect_rgba(SDL_Renderer* renderer, RectF rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

SOREN_EXPORT void draw_filled_rect_color(SDL_Renderer* renderer, RectF rect, SDL_Color color) {
    draw_filled_rect_rgba(renderer, rect, COLOR_DECONSTRUCT(color));
}

static void draw_circle_pixels(SDL_Renderer* renderer, Vector center, float x, float y) {
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

SOREN_EXPORT void draw_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    float x = 0;
    float y = radius;
    float d = 3 - 2 * radius;
    do {
        draw_circle_pixels(renderer, position, x, y);

        x++;
        if (d > 0) {
            y -= 1;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    } while (x <= y);
}

SOREN_EXPORT void draw_circle_color(SDL_Renderer* renderer, Vector position, float radius, SDL_Color color) {
    draw_circle_rgba(renderer, position, radius, COLOR_DECONSTRUCT(color));
}

SOREN_EXPORT void draw_filled_circle_rgba(SDL_Renderer* renderer, Vector position, float radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}

SOREN_EXPORT void draw_filled_circle_color(SDL_Renderer* renderer, Vector position, float radius, SDL_Color color) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}

SOREN_EXPORT void draw_line_rgba(SDL_Renderer* renderer, Vector start, Vector end, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
}

SOREN_EXPORT void draw_line_color(SDL_Renderer* renderer, Vector start, Vector end, SDL_Color color) {
    draw_line_rgba(renderer, start, end, COLOR_DECONSTRUCT(color));
}

SOREN_EXPORT void draw_filled_line_rgba(SDL_Renderer* renderer, Vector start, Vector end, float width, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    draw_filled_line_color(renderer, start, end, width, (SDL_Color){ r, g, b, a });    
}

SOREN_EXPORT void draw_filled_line_color(SDL_Renderer* renderer, Vector start, Vector end, float width, SDL_Color color) {
    soren_assert(width >= 1);

    if (width == 1) {
        draw_line_color(renderer, start, end, color);
        return;
    }

    float radius = width / 2;
    Vector perp = vector_perpendicular(start, end);
    perp = vector_normalize(perp);

    Vector top = vector_multiply_scalar(perp, radius);
    Vector bottom = vector_negate(top);

    SDL_Vertex vertices[4] = {
        {
            .color = color,
            .position = vector_add(start, top)
        },
        {
            .color = color,
            .position = vector_add(end, top)
        }, 
        {
            .color = color,
            .position = vector_add(end, bottom)
        },
        {
            .color = color,
            .position = vector_add(start, bottom)
        }
    };

    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(color.a));
    SDL_SetRenderDrawColor(renderer, COLOR_DECONSTRUCT(color));
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

SOREN_EXPORT void draw_point_rgba(SDL_Renderer* renderer, Vector point, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawBlendMode(renderer, PRIMITIVE_BLEND(a));
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderPoint(renderer, point.x, point.y);
}

SOREN_EXPORT void draw_point_color(SDL_Renderer* renderer, Vector point, SDL_Color color) {
    draw_point_rgba(renderer, point, COLOR_DECONSTRUCT(color));
}

SOREN_EXPORT void draw_arc_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}

SOREN_EXPORT void draw_arc_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, SDL_Color color) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}

SOREN_EXPORT void draw_filled_arc_rgba(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}

SOREN_EXPORT void draw_filled_arc_color(SDL_Renderer* renderer, Vector position, float radius, float start_angle, float end_angle, float thickness, SDL_Color color) {
    throw(NotImplementedException, __FUNCTION__ " not implemented");
}