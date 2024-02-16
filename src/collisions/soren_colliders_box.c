#include <collisions/soren_colliders.h>
#include <collisions/soren_collisions.h>

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

BoxCollider* box_collider_create(float width, float height) {
    BoxCollider* collider = soren_malloc(sizeof(*collider));
    if (!collider)
        return NULL;

    box_collider_init(collider, width, height);
    return collider;
}

void box_collider_init(BoxCollider* collider, float width, float height) {
    assert(collider);
    assert(width > 0);
    assert(height > 0);
    Vector points[4] = {0};
    collisions_build_box(points, width, height);
    polygon_collider_init((PolygonCollider*)collider, points, 4);

    collider->size = vector_create(width, height);
}

float box_collider_rotation(BoxCollider* box) {
    return polygon_collider_rotation((PolygonCollider*)box);
}

void box_collider_set_rotation(BoxCollider* box, float rotation) {
    polygon_collider_set_rotation((PolygonCollider*)box, rotation);
}

float box_collider_scale(BoxCollider* box) {
    return polygon_collider_scale((PolygonCollider*)box);
}

void box_collider_set_scale(BoxCollider* box, float scale) {
    polygon_collider_set_scale((PolygonCollider*)box, scale);
}

Vector box_collider_position(BoxCollider* box) {
    return polygon_collider_position((PolygonCollider*)box);
}

void box_collider_set_position(BoxCollider* box, Vector position) {
    polygon_collider_set_position((PolygonCollider*)box, position);
}

Vector box_collider_center(BoxCollider* box) {
    return polygon_collider_center((PolygonCollider*)box);
}

Vector box_collider_original_center(BoxCollider* box) {
    return polygon_collider_original_center((PolygonCollider*)box);
}

void box_collider_set_original_center(BoxCollider* box, Vector center) {
    polygon_collider_set_original_center((PolygonCollider*)box, center);
}

Vector box_collider_size(BoxCollider* box) {
    return vector_create(
        box->size.x * box->base.scale,
        box->size.y * box->base.scale);
}

float box_collider_get_width(BoxCollider* box) {
    return box->size.x * box->base.scale;
}

float box_collider_get_height(BoxCollider* box) {
    return box->size.x * box->base.scale;
}

Vector box_collider_original_size(BoxCollider* box) {
    return box->size;
}

void box_collider_original_set_size(BoxCollider* box, Vector size) {
    box_collider_set_size_impl(box, size.x, size.y);
}

float box_collider_original_get_width(BoxCollider* box) {
    return box->size.x;
}

void box_collider_original_set_width(BoxCollider* box, float width) {
    box_collider_set_size_impl(box, width, box->size.y);
}

float box_collider_original_get_height(BoxCollider* box) {
    return box->size.y;
}

void box_collider_original_set_height(BoxCollider* box, float height) {
    box_collider_set_size_impl(box, box->size.x, height);
}

Vector* box_collider_points(BoxCollider* box, int* out_count) {
    return polygon_collider_points((PolygonCollider*)box, out_count);
}

RectF box_collider_bounds(BoxCollider* box) {
    return polygon_collider_bounds((PolygonCollider*)box);
}

Vector* box_collider_edge_normals(BoxCollider* box, int* out_count) {
    return polygon_collider_edge_normals((PolygonCollider*)box, out_count);
}

bool box_collider_overlaps_rect(BoxCollider* collider, RectF rect) {
    if (collider_rotation(collider) == 0) {
        return collision_box_to_rect(collider, rect);
    } else {
        return polygon_collider_overlaps_rect((PolygonCollider*)collider, rect);
    }
}

bool box_collider_overlaps_collider(BoxCollider* collider, Collider* other) {
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

bool box_collider_overlaps_line(BoxCollider* collider, Vector start, Vector end) {
    return collision_segment_to_poly(start, end, (PolygonCollider*)collider);
}

bool box_collider_contains_point(BoxCollider* collider, Vector point) {
    if (collider_rotation(collider) == 0) {
        return collision_point_to_box(point, collider);
    } else {
        return polygon_collider_contains_point((PolygonCollider*)collider, point);
    }
}

bool box_collider_collides_rect(BoxCollider* collider, RectF rect, CollisionResult* out_result) {
    if (collider_rotation(collider) == 0) {
        return collision_box_to_rect_ext(collider, rect, out_result);
    } else {
        return polygon_collider_collides_rect((PolygonCollider*)collider, rect, out_result);
    }
}

bool box_collider_collides_collider(BoxCollider* collider, Collider* other, CollisionResult* out_result, RaycastHit* out_hit) {
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

bool box_collider_collides_line(BoxCollider* collider, Vector start, Vector end, RaycastHit* out_result) {
    return collision_segment_to_poly_ext(start, end, (PolygonCollider*)collider, out_result);
}

bool box_collider_collides_point(BoxCollider* collider, Vector point, CollisionResult* out_result) {
    if (collider_rotation(collider) == 0) {
        bool result = collision_point_to_box_ext(point, collider, out_result);
        if (result && out_result) {
            collision_result_invert(out_result);
        }

        return result;
    }

    return polygon_collider_collides_point((PolygonCollider*)collider, point, out_result);
}