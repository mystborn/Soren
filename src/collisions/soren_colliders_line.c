#include <collisions/soren_colliders.h>

static void line_collider_clean(LineCollider* line) {
    line->dirty = false;
    Vector start = vector_multiply_scalar(line->original_start, line->scale);
    Vector end = vector_multiply_scalar(line->original_end, line->scale);
    Vector pivot = vector_multiply_scalar(line->original_pivot, line->scale);

    if (line->rotation != 0) {
        float sin = SDL_sinf(line->rotation);
        float cos = SDL_cosf(line->rotation);

        start = vector_subtract(start, pivot);
        start = vector_create(start.x * cos - start.y * sin, start.x * sin + start.y * cos);
        start = vector_add(start, pivot);

        end = vector_subtract(end, pivot);
        end = vector_create(end.x * cos - end.y * sin, end.x * sin + end.y * cos);
        end = vector_add(end, pivot);
    }

    line->start = start;
    line->end = end;
    line->pivot = pivot;

    Vector top_left = vector_min(start, end);
    Vector bottom_right = vector_max(start, end);

    line->bounding_box = (RectF) {
        top_left.x,
        top_left.y,
        bottom_right.x - top_left.x,
        bottom_right.y - top_left.y
    };
}

float line_collider_rotation(LineCollider* line) {
    return line->rotation;
}

void line_collider_set_rotation(LineCollider* line, float rotation) {
    if (rotation == line->rotation)
        return;

    line->rotation = rotation;
    line->dirty = true;
}

float line_collider_scale(LineCollider* line) {
    return line->scale;
}

void line_collider_set_scale(LineCollider* line, float scale) {
    if (scale == line->scale)
        return;

    line->scale = scale;
    line->dirty = true;
}

Vector line_collider_position(LineCollider* line) {
    return line->position;
}

void line_collider_set_position(LineCollider* line, Vector position) {
    if (vector_equals(position, line->position))
        return;

    line->position = position;
    line->dirty = true;
}

RectF line_collider_bounds(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    RectF bounds = line->bounding_box;
    bounds.x += line->position.x;
    bounds.y += line->position.y;
    return bounds;
}

Vector line_collider_start(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->start;
}

Vector line_collider_end(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->end;
}

Vector line_collider_pivot(LineCollider* line) {
    if (line->dirty) {
        line_collider_clean(line);
    }

    return line->pivot;
}

Vector line_collider_adjusted_start(LineCollider* line) {
    Vector start = line_collider_start(line);
    return vector_add(start, line->position);
}

Vector line_collider_adjusted_end(LineCollider* line) {
    Vector end = line_collider_end(line);
    return vector_add(end, line->position);
}

Vector line_collider_original_start(LineCollider* line) {
    return line->original_start;
}

void line_collider_set_original_start(LineCollider* line, Vector start) {
    if (vector_equals(start, line->original_start))
        return;

    line->original_start = start;
    line->dirty = true;
}

Vector line_collider_original_end(LineCollider* line) {
    return line->original_end;
}

void line_collider_set_original_end(LineCollider* line, Vector end) {
    if (vector_equals(end, line->original_end))
        return;

    line->original_end = end;
    line->dirty = true;
}

Vector line_collider_original_pivot(LineCollider* line) {
    return line->original_pivot;
}

void line_collider_set_original_pivot(LineCollider* line, Vector pivot) {
    if (vector_equals(pivot, line->original_pivot))
        return;

    line->original_pivot = pivot;
    line->dirty = true;
}