#ifndef SOREN_RESOURCES_SOREN_JSON_H
#define SOREN_RESOURCES_SOREN_JSON_H

#include "../soren_std.h"
#include "../soren_math.h"
#include "../graphics/soren_padding.h"
#include "../external/parson.h"

SOREN_EXPORT Point soren_json_parse_point(JSON_Value* element);

/**
 * Parses a JSON element into a Vector value.
 * 
 * @details
 * Any of the following forms are allowed:
 * 
 * "32"             => vector_create(32, 32)
 * "32, 48"         => vector_create(32, 48)
 * [32]             => vector_create(32, 32)
 * [32, 48]         => vector_create(32, 48)
 * 32               => vector_create(32, 32)
 * { x: 32, y: 48 } => vector_create(32, 48)
 * 
 * @param element The JSON element to parse into a Vector value.
 * @return The parsed Vector
 * 
 * @throw IllegalArgumentException - The format doesn't match one of the above.
 */
SOREN_EXPORT Vector soren_json_parse_vector(JSON_Value* element);

/**
 * Parses a JSON element into a Padding value.
 * 
 * @details
 * Any of the following forms are allowed:
 * 10                                                   => padding_create_uniform(10)
 * "10"                                                 => padding_create_uniform(10)
 * "10, 20"                                             => padding_create_sides(10, 20)
 * "10, 20, 30, 40"                                     => padding_create(10, 20, 30, 40)
 * { "top": 10, "left": 20, "bottom": 30, "right": 40 } => padding_create(10, 20, 30, 40)
 * [10]                                                 => padding_create_uniform(10)
 * [10, 20]                                             => padding_create_sides(10, 20)
 * [10, 20, 30, 40]                                     => padding_create(10, 20, 30, 40)
 * 
 * @param element The JSON to parse into a Padding value.
 * @return The parsed Padding.
 * 
 * @throw JsonException - The format doesn't match one of the above.
 * @throw IllegalArgumentException - Failed to parse a string into a Padding value.
 */
SOREN_EXPORT Padding soren_json_parse_padding(JSON_Value* element);

SOREN_EXPORT Rect soren_json_parse_rect(JSON_Object* obj);

SOREN_EXPORT RectF soren_json_parse_rectf(JSON_Object* obj);

#endif