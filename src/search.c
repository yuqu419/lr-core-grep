#include "constant.h"
#include "error.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
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
void handle_buf(char buf[BUFFER_SIZE], size_t read_byte, size_t *match_count,
                size_t *match_count_ignore, m_string *single_line,
                match_line lines[MAX_LINE_COUNT], char *pattern,
                size_t *line_count) {
        char *remaining_ptr = buf;
        size_t remaining_byte = read_byte;
        while (true) {
                char *ptr = strchr(remaining_ptr, '\n');
                if (ptr) {
                        (*line_count)++;
                        string_concat(single_line, remaining_ptr,
                                      ptr - remaining_ptr);
                        if (find_pattern(single_line, pattern)) {
                                string_concat(
                                    &(lines[*match_count_ignore].content),
                                    single_line->data, single_line->size);
                                lines[*match_count_ignore].number = *line_count;
                                lines[*match_count_ignore].ignorance = false;

                                (*match_count)++;
                                (*match_count_ignore)++;
                        } else {
                                if (find_pattern_ignore(single_line, pattern)) {
                                        string_concat(
                                            &(lines[*match_count_ignore]
                                                  .content),
                                            single_line->data,
                                            single_line->size);
                                        lines[*match_count_ignore].number =
                                            *line_count;
                                        lines[*match_count_ignore].ignorance =
                                            true;
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
int collect_file(char *file, char *pattern, match_line lines[MAX_LINE_COUNT],
                 size_t *match_count, size_t *match_count_ignore) {
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
                        break;
                }
                handle_buf(buf, read_byte, match_count, match_count_ignore,
                           &single_line, lines, pattern, &line_count);
        }
        /*
        处理当文件不以\n结尾的情况
        */
        if (find_pattern(&single_line, pattern)) {
                string_concat(&(lines[*match_count_ignore].content),
                              single_line.data, single_line.size);
                lines[*match_count_ignore].number = ++line_count;
                lines[*match_count_ignore].ignorance = false;
                (*match_count)++;
                (*match_count_ignore)++;
        } else {
                if (find_pattern_ignore(&single_line, pattern)) {
                        string_concat(&(lines[*match_count_ignore].content),
                                      single_line.data, single_line.size);
                        lines[*match_count_ignore].number = ++line_count;
                        lines[*match_count_ignore].ignorance = true;
                        (*match_count_ignore)++;
                }
        }
cleanup:
        string_destory(&single_line);
        close(fd);
        return ret;
}
/*
match_counts,
match_counts_ignore全初始化为0
*/
int collect_all(char (*files)[FILE_NAME_SIZE], size_t file_count, char *pattern,
                match_line (*matches)[MAX_LINE_COUNT],
                size_t match_counts[file_count],
                size_t match_counts_ignorace[file_count]) {
        int ret = 0;
        for (size_t i = 0; i < file_count; i++) {
                match_counts[i] = 0;
                match_counts_ignorace[i] = 0;
                ret |=
                    collect_file(files[i], pattern, matches[i],
                                 match_counts + i, match_counts_ignorace + i);
        }
        return ret;
}