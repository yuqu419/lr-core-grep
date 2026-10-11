#include "arrange.h"
#include "constant.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static int set_none_info(size_t file_count, char (*files)[FILE_NAME_SIZE],
                         match_line *matches[file_count], m_string *output,
                         size_t match_counts[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {

                for (size_t j = 0; j < match_counts[i]; j++) {

                        match_line cur = matches[i][j];
                        if (cur.ignorance)
                                continue;
                        matched = 1;
                        if (file_count != 1) {
                                string_concat(output, files[i],
                                              strlen(files[i]));
                                string_concat(output, ":", 1);
                        }

                        string_concat(output, cur.content.data,
                                      cur.content.size);
                        string_concat(output, "\n", 1);
                }
        }
        return matched;
}
static int set_ignore_info(size_t file_count, char (*files)[FILE_NAME_SIZE],
                           match_line *matches[file_count], m_string *output,
                           size_t match_counts_ignore[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {
                for (size_t j = 0; j < match_counts_ignore[i]; j++) {
                        match_line cur = matches[i][j];
                        matched = 1;
                        if (file_count != 1) {
                                string_concat(output, files[i],
                                              strlen(files[i]));
                                string_concat(output, ":", 1);
                        }
                        string_concat(output, cur.content.data,
                                      cur.content.size);
                        string_concat(output, "\n", 1);
                }
        }

        return matched;
}
static int set_number_info(size_t file_count, char (*files)[FILE_NAME_SIZE],
                           match_line *matches[file_count], m_string *output,
                           size_t match_counts[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {
                for (size_t j = 0; j < match_counts[i]; j++) {
                        match_line cur = matches[i][j];
                        if (cur.ignorance)
                                continue;
                        matched = 1;
                        if (file_count != 1) {
                                string_concat(output, files[i],
                                              strlen(files[i]));
                                string_concat(output, ":", 1);
                        }
                        char temp[20] = {0};
                        sprintf(temp, "%lu", cur.number);
                        string_concat(output, temp, strlen(temp));
                        string_concat(output, ":", 1);
                        string_concat(output, cur.content.data,
                                      cur.content.size);
                        string_concat(output, "\n", 1);
                }
        }
        return matched;
}
static int set_number_ignore_info(size_t file_count,
                                  char (*files)[FILE_NAME_SIZE],
                                  match_line *matches[file_count],
                                  m_string *output,
                                  size_t match_counts_ignore[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {
                for (size_t j = 0; j < match_counts_ignore[i]; j++) {
                        match_line cur = matches[i][j];
                        matched = 1;
                        if (file_count != 1) {
                                string_concat(output, files[i],
                                              strlen(files[i]));
                                string_concat(output, ":", 1);
                        }
                        char temp[20] = {0};
                        sprintf(temp, "%lu", cur.number);
                        string_concat(output, temp, strlen(temp));
                        string_concat(output, ":", 1);
                        string_concat(output, cur.content.data,
                                      cur.content.size);
                        string_concat(output, "\n", 1);
                }
        }
        return matched;
}
static int set_count_info(size_t file_count, char (*files)[FILE_NAME_SIZE],
                          match_line *matches[file_count], m_string *output,
                          size_t match_counts[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {
                if (file_count != 1) {
                        string_concat(output, files[i], strlen(files[i]));
                        string_concat(output, ":", 1);
                }
                char temp[20] = {0};
                sprintf(temp, "%lu", match_counts[i]);
                string_concat(output, temp, strlen(temp));
                matched |= (match_counts[i] != 0);
                string_concat(output, "\n", 1);
        }
        return matched;
}
static int set_count_ignore_info(size_t file_count,
                                 char (*files)[FILE_NAME_SIZE],
                                 match_line *matches[file_count],
                                 m_string *output,
                                 size_t match_counts_ignore[file_count]) {
        int matched = 0;
        for (size_t i = 0; i < file_count; i++) {
                if (file_count != 1) {
                        string_concat(output, files[i], strlen(files[i]));
                        string_concat(output, ":", 1);
                }
                char temp[20] = {0};
                sprintf(temp, "%lu", match_counts_ignore[i]);
                string_concat(output, temp, strlen(temp));
                matched |= (match_counts_ignore[i] != 0);

                string_concat(output, "\n", 1);
        }
        return matched;
}

int arrange_info(option opt, size_t file_count, char (*files)[FILE_NAME_SIZE],
                 match_line *matches[file_count], m_string *output,
                 size_t match_counts[file_count],
                 size_t match_counts_ignore[file_count]) {
        int ret;
        switch (opt) {
        case NONE:
                ret = set_none_info(file_count, files, matches, output,
                                    match_counts);
                break;
        case NUMBER:
                ret = set_number_info(file_count, files, matches, output,
                                      match_counts);
                break;
        case COUNT:
                ret = set_count_info(file_count, files, matches, output,
                                     match_counts);
                break;
        case IGNORE_AND_COUNT:
                ret = set_count_ignore_info(file_count, files, matches, output,
                                            match_counts_ignore);
                break;
        case IGNORE:
                ret = set_ignore_info(file_count, files, matches, output,
                                      match_counts_ignore);
                break;
        case IGNORE_AND_NUMBER:
                ret = set_number_ignore_info(file_count, files, matches, output,
                                             match_counts_ignore);
                break;
        default:
                break;
        }
        return ret;
}
