#ifndef CONSTANT_H
#define CONSTANT_H
#include <stdbool.h>
#include <stddef.h>
#include "mstring.h"
#define FILE_NAME_SIZE 256
#define MAX_FILE_COUNT 256
#define MAX_LINE_COUNT 48
#define PATTERN_SIZE 1024
#define BUFFER_SIZE 4096 
typedef enum {
    IGNORE,
    NUMBER,
    COUNT,
    IGNORE_AND_NUMBER,
    IGNORE_AND_COUNT,
    NONE
} option;
typedef struct{
    bool ignorance; //是否是忽略大小写匹配到的
    size_t number;//行号
    m_string content;//数据
} match_line;
#endif
