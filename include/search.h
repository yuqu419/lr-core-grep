
#ifndef SEARCH_H
#define SEARCH_H
// #include "mstring.h"

// typedef struct {
//         long column;
//         m_string *str;
// } line_data;

// int collect_lines(char *file, char *pattern, char *option, m_string
// *output,bool* no_match);
#include "constant.h"
int collect_all(char (*files)[FILE_NAME_SIZE], size_t file_count,
                 char *pattern, match_line (*matches)[MAX_LINE_COUNT],
                 size_t match_counts[file_count],
                 size_t match_counts_ignorace[file_count]);
#endif