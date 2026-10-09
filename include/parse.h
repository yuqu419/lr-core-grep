#ifndef PARSE_H
#define PARSE_H
#include "constant.h"

typedef enum{
    COUNT,
    IGNORE,
    NUMBER,
    IGNORE_AND_NUMBER
} opt;

int parse_args(char **argv, int argc, char *options, char *pattern,
               char files[][FILE_NAME_LENGTH], int *file_count);
#endif