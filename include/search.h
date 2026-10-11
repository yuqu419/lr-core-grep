
#ifndef SEARCH_H
#define SEARCH_H
#include "constant.h"
int collect_all(char (*files)[FILE_NAME_SIZE], size_t file_count, char *pattern,
                match_line *matches[file_count],
                size_t match_counts[file_count],
                size_t match_counts_ignorace[file_count],
                size_t match_cap[MAX_FILE_COUNT]);
#endif