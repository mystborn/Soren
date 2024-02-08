#include <collisions/soren_colliders.h>

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
        return;

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