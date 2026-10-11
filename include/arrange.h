#ifndef ARRANGE_H
#define ARRANGE_H
#include "constant.h"
int arrange_info(option opt, size_t file_count, char (*files)[FILE_NAME_SIZE],
                  match_line matches[file_count][MAX_LINE_COUNT],
                  m_string *output, size_t match_counts[file_count],
                  size_t match_counts_ignore[file_count]);
#endif