#include <soren_enum_parser.h>

#include <generic_map.h>
#include <string.h>

#include "soren_init.h"

MAP_DEFINE_H(SorenEnumMap, soren_enum_map, const char*, int64_t)
MAP_DEFINE_C(SorenEnumMap, soren_enum_map, const char*, int64_t, gds_fnv32_case_insensitive, stricmp)

SorenEnumMap enum_parser;

void soren_enum_parser_init(void) {
    soren_enum_map_init(&enum_parser);
    sprite_update_mode_enum_init();
}

void soren_enum_parser_add(const char* name, int64_t value) {
    if (!soren_enum_map_add(&enum_parser, name, value)) {
        throw(IllegalArgumentException, "Already added enum name to enum parser");
    }
}

int64_t soren_parse_enum(const char* name) {
    int64_t value;
    if (!soren_try_parse_enum(name, &value)) {
        throw(IllegalArgumentException, "No enum with the given name defined.");
    }

    return value;
}

bool soren_try_parse_enum(const char* name, int64_t* value) {
    return soren_enum_map_try_get(&enum_parser, name, value);
}