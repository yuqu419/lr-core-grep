// #ifndef PARSE_H
// #define PARSE_H
#include "constant.h"
#include <stddef.h>

int parse_args(char **argv, int argc, option *opt, char *pattern,
               char files[][FILE_NAME_SIZE], size_t *file_count);
// #endif