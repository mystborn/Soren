#ifndef SOREN_GRAPHICS_SOREN_PADDING_H
#define SOREN_GRAPHICS_SOREN_PADDING_H

#include "../soren_std.h"
#include "../soren_math.h"

typedef struct Padding {
    int top;
    int left;
    int bottom;
    int right;
} Padding;

#define PADDING_EMPTY (Padding){ 0, 0, 0, 0 }

static inline int padding_top(Padding* padding) {
    return padding->top;
}

static inline int padding_left(Padding* padding) {
    return padding->left;
}

static inline int padding_bottom(Padding* padding) {
    return padding->bottom;
}

static inline int padding_right(Padding* padding) {
    return padding->right;
}

static inline void padding_set_top(Padding* padding, int value) {
    padding->top = value;
}

static inline void padding_set_left(Padding* padding, int value) {
    padding->left = value;
}

static inline void padding_set_bottom(Padding* padding, int value) {
    padding->bottom = value;
}

static inline void padding_set_right(Padding* padding, int value) {
    padding->right = value;
}

static inline int padding_width(Padding* padding) {
    return padding->left + padding->right;
}

static inline int padding_height(Padding* padding) {
    return padding->top + padding->bottom;
}

static inline Point padding_size(Padding* padding) {
    return (Point){ padding_width(padding), padding_height(padding) };
}

static inline bool padding_not_empty(Padding* padding) {
    return padding->top != 0
        || padding->left != 0
        || padding->bottom != 0
        || padding->right != 0;
}

static inline Padding padding_create(int top, int left, int bottom, int right) {
    return (Padding){ top, left, bottom, right };
}

static inline Padding padding_create_sides(int top_bottom, int left_right) {
    return (Padding){ top_bottom, left_right, top_bottom, left_right };
}

static inline Padding padding_create_uniform(int value) {
    return (Padding){ value, value, value, value };
}

static inline bool padding_equals(Padding* left, Padding* right) {
    return left->top == right->top
        && left->left == right->left
        && left->bottom == right->bottom
        && left->right == right->right;
}

static inline uint32_t padding_hash(Padding* padding) {
    return (((((padding->left * 397) ^ padding->top) * 397) ^ padding->right) * 397) ^ padding->bottom;
}

static inline String* padding_to_string(Padding* padding, String* str) {
    return string_format(
        str,
        "Padding { Top: %d, Left: %d, Bottom: %d, Right: %d }",
        padding->top,
        padding->left,
        padding->bottom,
        padding->right);
}

static inline Padding padding_parse(String* str) {
    String result[4];
    int count;
    Padding padding;
    string_split_cstr(str, ",", result, 4, &count, true, true);
    switch (count) {
        case 1:
            padding = padding_create_uniform(atoi(string_data(result)));
            break;
        case 2:
            padding = padding_create_sides(
                atoi(string_data(result)),
                atoi(string_data(result + 1)));
            break;
        case 4:
            padding = padding_create(
                atoi(string_data(result)),
                atoi(string_data(result + 1)),
                atoi(string_data(result + 2)),
                atoi(string_data(result + 3)));
            break;
        default:
            for(int i = 0; i < count; i++) {
                string_free_resources(result + i);
            }
            throw(IllegalArgumentException, "Could not parse the padding string");
            break;
    }

    for(int i = 0; i < count; i++) {
        string_free_resources(result + i);
    }

    return padding;
}

#endif