#ifndef SOREN_ENUM_PARSER_H
#define SOREN_ENUM_PARSER_H

#include "soren_std.h"
#include <stdint.h>

void soren_enum_parser_add(const char* name, int64_t value);

int64_t soren_parse_enum(const char* name);
bool soren_try_parse_enum(const char* name, int64_t* value);

#endif