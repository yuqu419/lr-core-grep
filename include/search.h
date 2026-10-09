
#ifndef SEARCH_H
#define SEARCH_H
#include "mstring.h"

typedef struct {
        long column;
        m_string *str;
} line_data;

int collect_lines(char *file, char *pattern, char *option, m_string **output,bool* no_match);
#endif