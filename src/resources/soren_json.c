#include <resources/soren_json.h>

SOREN_EXPORT Vector soren_json_parse_vector(JSON_Value* element) {
    Vector vector;
    float number;
    float x;
    float y;

    switch (json_value_get_type(element)) {
        case JSONNumber:
            number = (float)json_number(element);
            vector = vector_create(number, number);
            break;
        case JSONString:
            String str = string_create(json_string(element));
            String split_results[2];
            int split_count;
            string_split(&str, ",", split_results, 2, &split_count, true, true);
            switch (split_count) {
                case 1:
                    number = (float)atof(string_data(split_results));
                    vector = vector_create(number, number);
                    break;
                case 2:
                    x = (float)atof(string_data(split_results));
                    y = (float)atof(string_data(split_results + 1));
                    vector = vector_create(x, y);
                    break;
                default:
                    for (int i = 0; i < split_count; i++) {
                        string_free_resources(split_results + i);
                    }
                    string_free_resources(&str);
                    throw(JsonException, "Invalid vector in the JSON");
                    break;
            }
            for (int i = 0; i < split_count; i++) {
                string_free_resources(split_results + i);
            }
            string_free_resources(&str);
            break;
        case JSONArray:
            JSON_Array* array = json_array(element);
            int count = json_array_get_count(array);
            for (int i = 0; i < count; i++) {
                if (json_value_get_type(json_array_get_value(array, i)) != JSONNumber) {
                    throw(JsonException, "Can only parse number arrays into Vectors");
                }
            }
            switch (count) {
                case 1:
                    number = json_array_get_number(array, 0);
                    vector = vector_create(number, number);
                    break;
                case 2:
                    x = json_array_get_number(array, 0);
                    y = json_array_get_number(array, 1);
                    vector = vector_create(x, y);
                    break;
                default:
                    throw(JsonException, "Invalid number of numbers in array to parse into Vector");
                    break;
            }
            break;
        case JSONObject:
            JSON_Object* obj = json_object(element);
            if (!json_object_has_value_of_type(obj, "x", JSONNumber)
                || !json_object_has_value_of_type(obj, "y", JSONNumber))
            {
                throw(JsonException, "Missing 'x' or 'y' in Vector JSON object");
            }

            x = json_object_get_number(obj, "x");
            y = json_object_get_number(obj, "y");
            vector = vector_create(x, y);
            break;
        default:
            throw(JsonException, "Invalid JSON type to parse into Vector");
            break;
    }

    return vector;
}

SOREN_EXPORT Padding soren_json_parse_padding(JSON_Value* element) {
    Padding padding;

    switch(json_value_get_type(element)) {
        case JSONNumber:
            padding = padding_create_uniform((int)json_number(element));
            break;
        case JSONString:
            String str = string_create(json_string(element));
            padding = padding_parse(&str);
            string_free_resources(&str);
            break;
        case JSONObject:
            JSON_Object* obj = json_object(element);
            if(json_object_has_value(obj, "top")) {
                padding.top = (int)json_object_get_number(obj, "top");
            }
            
            if(json_object_has_value(obj, "left")) {
                padding.left = (int)json_object_get_number(obj, "left");
            }
            
            if(json_object_has_value(obj, "bottom")) {
                padding.bottom = (int)json_object_get_number(obj, "bottom");
            }
            
            if(json_object_has_value(obj, "right")) {
                padding.right = (int)json_object_get_number(obj, "right");
            }

            if (padding.top < 0 || padding.left < 0 || padding.bottom < 0 || padding.right < 0) {
                throw(JsonException, "Padding values cannot be less than zero");
            }
            break;
        case JSONArray:
            JSON_Array* array = json_array(element);
            size_t count = json_array_get_count(array);
            switch (count) {
                case 1:
                    padding = padding_create_uniform((int)json_array_get_number(array, 0));
                    break;
                case 2:
                    padding_create_sides(
                        (int)json_array_get_number(array, 0),
                        (int)json_array_get_number(array, 1));
                case 4:
                    padding_create(
                        (int)json_array_get_number(array, 0),
                        (int)json_array_get_number(array, 1),
                        (int)json_array_get_number(array, 2),
                        (int)json_array_get_number(array, 3));
                default:
                    throw(JsonException, "Invalid number of items in array to create Padding");
            }
        default:
            throw(JsonException, "Invalid JSON type to create padding");
    }

    return padding;
}