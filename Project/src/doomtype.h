

#pragma once

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>

typedef uint8_t byte;

#define DIR_SEPARATOR   '\\'
#define DIR_SEPARATOR_S "\\"
#define PATH_SEPARATOR  ';'

#define arrlen(array)   (sizeof(array) / sizeof(*array))
