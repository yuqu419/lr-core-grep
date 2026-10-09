#include "search.h"
#include "constant.h"
#include "error.h"
#include "mstring.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
static void to_lower_case(char *str) {
        if (!str)
                return;
        for (int i = 0; str[i] != '\0'; i++) {
                if (str[i] >= 'A' && str[i] <= 'Z')
                        str[i] = tolower(str[i]);
        }
}
/*
调用者应保证参数均有效
会为每个读取的末尾添加\n
*/
static int stream_read(int fd, m_string **line_str, int *finish) {
        char buf[1] = {0};
        while (true) {
                ssize_t read_byte = read(fd, buf, 1);
                if (read_byte == 0) {
                        *finish = true;
                        push_back(line_str, '\n');
                        return SUCCESS;
                }
                if (read_byte == -1) {
                        if (errno == EINTR)
                                continue;
                        exit_code = 2;
                        strcpy(filename, "no-such-file");
                        return ERR_FILE_READ;
                }
                if (buf[0] == '\n') {
                        push_back(line_str, '\n');
                        break;
                }
                push_back(line_str, buf[0]);
        }
        return SUCCESS;
}
/*
功能：提供单独的一行，找出其是否满足条件
调用者应保证传参正常
*/
static int filter_pattern(m_string **line_str, const char *pattern,
                          int ignore_upper_lower) {
        char lower_pattern[strlen(pattern) + 1];
        char lower_single_line[strlen((*line_str)->data) + 1];
        strcpy(lower_pattern, pattern);
        strcpy(lower_single_line, (*line_str)->data);
        if (ignore_upper_lower) {
                to_lower_case(lower_pattern);
                to_lower_case(lower_single_line);
        }
        char *res = strstr(lower_single_line, lower_pattern);
        if (res != NULL) {
                return true;
        }
        return false;
}

/*
调用者保证参数安全可靠
每次读取一行内容，把去读取到的内容存入match_line内
保证line_str为空，且空间足够
*/
static int sift_line(int fd, char *pattern, m_string **line_str,
                     int ignore_upper_lower, line_data *match_line, int *finish,
                     size_t *line_count) {

        int res = stream_read(fd, line_str, finish);
        if (res != SUCCESS) {
                return res;
        }
        (*line_count)++;
        if (filter_pattern(line_str, pattern, ignore_upper_lower)) {
                *match_line =
                    (line_data){.str = *line_str, .column = *line_count};
        }
        return SUCCESS;
}
static int update_info(line_data data, m_string **output) {
        // reserve(output, (*output)->size + data.str->size);
        // strcat((*output)->data, data.str->data);
        concatenate(output, data.str->data);
        // update_size((*output));
        return SUCCESS;
}
static int update_info_with_number(line_data data, m_string **output) {
        char col[20] = {0};
        sprintf(col, "%lu:", data.column);

        reserve(output, (*output)->size + data.str->size + 20);
        strcat((*output)->data, col);
        strcat((*output)->data, data.str->data);
        update_size((*output));
        return SUCCESS;
}
static int update_info_count(m_string **output, size_t *count) {
        (*count)++;
        clear(*output);
        // long最长19位
        char col[20] = {0};
        sprintf(col, "%lu\n", *count);
        strcat((*output)->data, col);
        update_size(*output);
        return SUCCESS;
}
// static int output_check(m_string **output, char *option) {
//         if ((*output)->data[0] == '\0' && strchr(option, 'c') != NULL) {
//                 push_back(output, '0');
//                 push_back(output, '\n');
//         }
//         return SUCCESS;
// }
int collect_lines(char *file, char *pattern, char *option, m_string **output,
                  bool *no_match) {
        bool original = true;
        if ((*output)->data[0] == '\0' && strchr(option, 'i')) {
                push_back(output, '0');
                push_back(output, '\n');
        }
        int ret = SUCCESS;
        size_t line_count = 0;
        size_t match_count = 0;
        int fd = open(file, O_RDONLY, 0777);
        int finish = 0;
        if (fd == -1) {
                exit_code = 2;
                strcpy(filename, file);
                return ERR_FILE_OPEN;
        }
        m_string *line_str = string_init(BUFFER_SIZE);
        line_data data = (line_data){.str = NULL};
        int ignore_upper_lower = 0;
        if (strchr(option, 'i') != NULL)
                ignore_upper_lower = true;
        while (true) {
                int res = sift_line(fd, pattern, &line_str, ignore_upper_lower,
                                    &data, &finish, &line_count);
                if (res != SUCCESS) {
                        ret = res;
                        goto cleanup;
                }
                if (data.str) {
                        if (original) {
                                clear(*output);
                                original = false;
                        }
                        if (strchr(option, 'c') != NULL) {
                                update_info_count(output, &match_count);
                        } else {
                                if (strchr(option, 'n') != NULL) {
                                        update_info_with_number(data, output);
                                } else {
                                        update_info(data, output);
                                }
                        }

                        *no_match = false;
                }
                data.str = NULL;
                clear(line_str);
                if (finish)
                        break;
        }

cleanup:
        string_destroy(line_str);
        close(fd);
        return ret;
}