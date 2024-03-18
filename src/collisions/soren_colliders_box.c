#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>
#include <collisions/soren_collision_utils.h>
#include <graphics/soren_primitives.h>

static void box_collider_set_size_impl(
    BoxCollider* box, 
    float width, 
    float height)
{
    box->size = vector_create(width, height);
    box->base.dirty = true;
    box->base.original_points[1].x = width;
    box->base.original_points[2] = vector_create(width, height);
    box->base.original_points[3].y = height;
}

SOREN_EXPORT BoxCollider* box_collider_create(float width, float height) {
    BoxCollider* collider = soren_malloc(sizeof(*collider));
    if (!collider)
        return NULL;

    box_collider_init(collider, width, height);
    return collider;
}

SOREN_EXPORT void box_collider_init(BoxCollider* collider, float width, float height) {
    soren_assert(collider);
    soren_assert(width > 0);
    soren_assert(height > 0);
    Vector points[4] = {0};
    collisions_build_box(points, width, height);
    polygon_collider_init((PolygonCollider*)collider, points, 4);

    collider->base.base.collider_type = COLLIDER_BOX;

    collider->size = vector_create(width, height);
}

SOREN_EXPORT float box_collider_rotation(BoxCollider* box) {
    return polygon_collider_rotation((PolygonCollider*)box);
}

SOREN_EXPORT void box_collider_set_rotation(BoxCollider* box, float rotation) {
    polygon_collider_set_rotation((PolygonCollider*)box, rotation);
}

SOREN_EXPORT float box_collider_scale(BoxCollider* box) {
    return polygon_collider_scale((PolygonCollider*)box);
}

SOREN_EXPORT void box_collider_set_scale(BoxCollider* box, float scale) {
    polygon_collider_set_scale((PolygonCollider*)box, scale);
}

SOREN_EXPORT Vector box_collider_position(BoxCollider* box) {
    return polygon_collider_position((PolygonCollider*)box);
}

SOREN_EXPORT void box_collider_set_position(BoxCollider* box, Vector position) {
    polygon_collider_set_position((PolygonCollider*)box, position);
}

SOREN_EXPORT Vector box_collider_center(BoxCollider* box) {
    return polygon_collider_center((PolygonCollider*)box);
}

SOREN_EXPORT Vector box_collider_original_center(BoxCollider* box) {
    return polygon_collider_original_center((PolygonCollider*)box);
}

SOREN_EXPORT void box_collider_set_original_center(BoxCollider* box, Vector center) {
    polygon_collider_set_original_center((PolygonCollider*)box, center);
}

SOREN_EXPORT Vector box_collider_size(BoxCollider* box) {
    return vector_create(
        box->size.x * box->base.scale,
        box->size.y * box->base.scale);
}

SOREN_EXPORT float box_collider_get_width(BoxCollider* box) {
    return box->size.x * box->base.scale;
}

SOREN_EXPORT float box_collider_get_height(BoxCollider* box) {
    return box->size.x * box->base.scale;
}

SOREN_EXPORT Vector box_collider_original_size(BoxCollider* box) {
    return box->size;
}

SOREN_EXPORT void box_collider_original_set_size(BoxCollider* box, Vector size) {
    box_collider_set_size_impl(box, size.x, size.y);
}

SOREN_EXPORT float box_collider_original_get_width(BoxCollider* box) {
    return box->size.x;
}

SOREN_EXPORT void box_collider_original_set_width(BoxCollider* box, float width) {
    box_collider_set_size_impl(box, width, box->size.y);
}

SOREN_EXPORT float box_collider_original_get_height(BoxCollider* box) {
    return box->size.y;
}

SOREN_EXPORT void box_collider_original_set_height(BoxCollider* box, float height) {
    box_collider_set_size_impl(box, box->size.x, height);
}

SOREN_EXPORT Vector* box_collider_points(BoxCollider* box, int* out_count) {
    return polygon_collider_points((PolygonCollider*)box, out_count);
}

SOREN_EXPORT RectF box_collider_bounds(BoxCollider* box) {
    return polygon_collider_bounds((PolygonCollider*)box);
}

SOREN_EXPORT void box_collider_debug_draw(BoxCollider* box, SDL_Renderer* renderer, SDL_FColor color) {
    if (collider_rotation(box) == 0) {
        draw_rect_color(renderer, box_collider_bounds(box), color);
    } else {
        polygon_collider_debug_draw((PolygonCollider*)box, renderer, color);
    }
}

SOREN_EXPORT Vector* box_collider_edge_normals(BoxCollider* box, int* out_count) {
    return polygon_collider_edge_normals((PolygonCollider*)box, out_count);
}

SOREN_EXPORT bool box_collider_overlaps_rect(BoxCollider* collider, RectF rect) {
    if (collider_rotation(collider) == 0) {
        return collision_box_to_rect(collider, rect);
    } else {
        return polygon_collider_overlaps_rect((PolygonCollider*)collider, rect);
    }
}

SOREN_EXPORT bool box_collider_overlaps_collider(BoxCollider* collider, Collider* other) {
    if (collider_rotation(collider) != 0) {
        return polygon_collider_overlaps_collider((PolygonCollider*)collider, other);
    }

    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return box_collider_overlaps_collider(collider, (Collider*)point->box);
            } else {
                return collision_point_to_box(collider_position(point), collider);
            }
        case COLLIDER_CIRCLE:
            return collision_circle_to_box((CircleCollider*)other, collider);
        case COLLIDER_BOX:
            BoxCollider* box = (BoxCollider*)other;
            if (collider_rotation(box) == 0) {
                return collision_box_to_box(collider, box);
            } else {
                return collision_polygon_to_polygon((PolygonCollider*)collider, (PolygonCollider*)box);
            }
    }

    return polygon_collider_overlaps_collider((PolygonCollider*)collider, other);
}

SOREN_EXPORT bool box_collider_overlaps_line(BoxCollider* collider, Vector start, Vector end) {
    return collision_segment_to_poly(start, end, (PolygonCollider*)collider);
}

SOREN_EXPORT bool box_collider_contains_point(BoxCollider* collider, Vector point) {
    if (collider_rotation(collider) == 0) {
        return collision_point_to_box(point, collider);
    } else {
        return polygon_collider_contains_point((PolygonCollider*)collider, point);
    }
}

SOREN_EXPORT bool box_collider_collides_rect(BoxCollider* collider, RectF rect, CollisionResult* out_result) {
    if (collider_rotation(collider) == 0) {
        return collision_box_to_rect_ext(collider, rect, out_result);
    } else {
        return polygon_collider_collides_rect((PolygonCollider*)collider, rect, out_result);
    }
}

SOREN_EXPORT bool box_collider_collides_collider(BoxCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
    if (collider_rotation(collider) != 0) {
        return polygon_collider_collides_collider((PolygonCollider*)collider, other, out_result, out_hit);
    }

    switch (other->collider_type) {
        case COLLIDER_POINT:
            PointCollider* point = (PointCollider*)other;
            if (point_collider_using_internal_collider(point)) {
                return box_collider_collides_collider(collider, (Collider*)point->box, out_result, out_hit);
            } else {
                return collision_point_to_box_ext(collider_position(point), collider, out_result);
            }
        case COLLIDER_CIRCLE:
            return collision_circle_to_box_ext((CircleCollider*)other, collider, out_result);
        case COLLIDER_BOX:
            BoxCollider* box = (BoxCollider*)other;
            if (collider_rotation(box) == 0) {
                return collision_box_to_box_ext(collider, box, out_result);
            } else {
                return collision_polygon_to_polygon_ext((PolygonCollider*)collider, (PolygonCollider*)box, out_result);
            }
    }

    return polygon_collider_collides_collider((PolygonCollider*)collider, other, out_result, out_hit);
}

SOREN_EXPORT bool box_collider_collides_line(BoxCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    return collision_segment_to_poly_ext(start, end, (PolygonCollider*)collider, out_result);
}

SOREN_EXPORT bool box_collider_collides_point(BoxCollider* collider, Vector point, CollisionResult* out_result) {
    if (collider_rotation(collider) == 0) {
        bool result = collision_point_to_box_ext(point, collider, out_result);
        if (result && out_result) {
            collision_result_invert(out_result);
        }

        return result;
    }

    return polygon_collider_collides_point((PolygonCollider*)collider, point, out_result);
}