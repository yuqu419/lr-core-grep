#include "constant.h"
#include "error.h"
#include "mstring.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
/*
收集单个文件信息
*/
static bool find_pattern(m_string *str, char *pattern) {
        char *res = strstr(str->data, pattern);
        if (res != NULL) {
                return true;
        }
        return false;
}
static bool find_pattern_ignore(m_string *str, char *pattern) {
        char *res = strcasestr(str->data, pattern);
        if (res != NULL) {
                return true;
        }
        return false;
}
int match_reserve(match_line **ptr, size_t *match_cap, size_t desire_cap) {
        if ((*match_cap) >= desire_cap)
                return 0;
        desire_cap = (*match_cap) * 2 + 1;
        void *new_ptr = realloc(*ptr, desire_cap * sizeof(match_line));
        if (new_ptr == NULL)
                return -1;
        (*ptr) = new_ptr;
        for (size_t i = *match_cap; i < desire_cap; i++) {
                string_init(&((*ptr)[i].content), BUFFER_SIZE);
        }
        (*match_cap) = desire_cap;
        return 0;
}

static void handle_buf(char buf[BUFFER_SIZE], size_t read_byte,
                       size_t *match_count, size_t *match_count_ignore,
                       m_string *single_line, match_line **lines, char *pattern,
                       size_t *line_count, size_t *match_cap) {
        char *remaining_ptr = buf;
        size_t remaining_byte = read_byte;
        while (true) {
                char *ptr = strchr(remaining_ptr, '\n');
                if (ptr) {
                        (*line_count)++;
                        string_concat(single_line, remaining_ptr,
                                      ptr - remaining_ptr);
                        if (find_pattern(single_line, pattern)) {
                                match_reserve(lines, match_cap,
                                              (*match_count_ignore) + 1);
                                string_concat(
                                    &((*lines)[*match_count_ignore].content),
                                    single_line->data, single_line->size);
                                (*lines)[*match_count_ignore].number =
                                    *line_count;
                                (*lines)[*match_count_ignore].ignorance = false;

                                (*match_count)++;
                                (*match_count_ignore)++;
                        } else {
                                if (find_pattern_ignore(single_line, pattern)) {
                                        match_reserve(lines, match_cap,
                                                      (*match_count_ignore) +
                                                          1);
                                        string_concat(
                                            &((*lines)[*match_count_ignore]
                                                  .content),
                                            single_line->data,
                                            single_line->size);
                                        (*lines)[*match_count_ignore].number =
                                            *line_count;
                                        (*lines)[*match_count_ignore]
                                            .ignorance = true;
                                        (*match_count_ignore)++;
                                }
                        }
                        string_clear(single_line);

                        remaining_byte -= (ptr - remaining_ptr) + 1;
                        remaining_ptr = ptr + 1;
                }

                else {
                        string_concat(single_line, remaining_ptr,
                                      remaining_byte);
                        break;
                }
                if (remaining_byte <= 0)
                        break;
        }
}
static int collect_file(char *file, char *pattern, match_line **lines,
                        size_t *match_count, size_t *match_count_ignore,
                        size_t *match_cap) {
        int ret = 0;
        int fd = open(file, O_RDONLY);
        m_string single_line;
        string_init(&single_line, BUFFER_SIZE);
        size_t line_count = 0;
        if (fd < 0) {
                report_err(1, file);
                ret = -1;
                goto cleanup;
        }
        struct stat fd_status;
        fstat(fd, &fd_status);
        if (S_ISDIR(fd_status.st_mode)) {
                errno = EISDIR;
                report_err(1, file);
                ret = -1;
                goto cleanup;
        }

        char buf[BUFFER_SIZE] = {0};
        while (true) {
                memset(buf, 0, BUFFER_SIZE);
                ssize_t read_byte = read(fd, buf, BUFFER_SIZE - 1);
                if (read_byte == 0) {
                        break;
                }
                if (read_byte == -1) {
                        if (errno == EINTR)
                                continue;
                        report_err(1, file);
                        ret = -1;
                        goto cleanup;
                }
                handle_buf(buf, read_byte, match_count, match_count_ignore,
                           &single_line, lines, pattern, &line_count,
                           match_cap);
        }
        /*
        处理当文件不以\n结尾的情况
        */
        if (find_pattern(&single_line, pattern)) {
                match_reserve(lines, match_cap, (*match_count_ignore) + 1);
                string_concat(&((*lines)[*match_count_ignore].content),
                              single_line.data, single_line.size);
                (*lines)[*match_count_ignore].number = ++line_count;
                (*lines)[*match_count_ignore].ignorance = false;
                (*match_count)++;
                (*match_count_ignore)++;
        } else {
                match_reserve(lines, match_cap, (*match_count_ignore) + 1);
                if (find_pattern_ignore(&single_line, pattern)) {
                        string_concat(&((*lines)[*match_count_ignore].content),
                                      single_line.data, single_line.size);
                        (*lines)[*match_count_ignore].number = ++line_count;
                        (*lines)[*match_count_ignore].ignorance = true;
                        (*match_count_ignore)++;
                }
        }
cleanup:
        string_destroy(&single_line);
        close(fd);
        return ret;
}
/*
match_counts,
match_counts_ignore全初始化为0
*/
int collect_all(char (*files)[FILE_NAME_SIZE], size_t file_count, char *pattern,
                match_line *matches[file_count],
                size_t match_counts[file_count],
                size_t match_counts_ignorace[file_count],
                size_t match_cap[MAX_FILE_COUNT]) {
        int ret = 0;
        for (size_t i = 0; i < file_count; i++) {
                ret |= collect_file(files[i], pattern, &matches[i],
                                    match_counts + i, match_counts_ignorace + i,
                                    match_cap + i);
        }
        return ret;
}