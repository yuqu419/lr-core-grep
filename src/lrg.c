#include "arrange.h"
#include "constant.h"
#include "parse.h"
#include "search.h"
#include <stddef.h>
#include <stdio.h>
/*
解析参数->收集信息->格式整理->打印输出
信息收集：
信息存储再什么内？
需要存储的三个数据：字符串，行号，和是否忽略了大小写





*/

static void init_lines(match_line (*matches)[MAX_LINE_COUNT],
                       size_t file_count) {

        for (size_t i = 0; i < file_count; i++) {
                for (size_t j = 0; j < MAX_LINE_COUNT; j++) {
                        string_init(&(matches[i][j].content), 1);
                }
        }
}
static void destroy_lines(match_line (*matches)[MAX_LINE_COUNT],
                          size_t file_count) {
        for (size_t i = 0; i < file_count; i++) {
                for (size_t j = 0; j < MAX_LINE_COUNT; j++) {
                        string_destory(&(matches[i][j].content));
                }
        }
}
int main(int argc, char *argv[]) {
        option opt = NONE;
        size_t file_count = 0;
        int ret = 0;
        char files[MAX_FILE_COUNT][FILE_NAME_SIZE] = {0};
        char pattern[PATTERN_SIZE] = {0};
        int res = parse_args(argv, argc, &opt, pattern, files, &file_count);
        if (res == -1) {
                return 2;
        }
        // for (size_t i = 0; i < file_count; i++) {
        //         printf("%s\n", files[i]);
        // }
        match_line matches[file_count][MAX_LINE_COUNT];
        init_lines(matches, file_count);
        size_t match_count[file_count];
        size_t match_count_ignore[file_count];
        m_string output;
        string_init(&output, BUFFER_SIZE);
        res = collect_all(files, file_count, pattern, matches, match_count,
                          match_count_ignore);
        if (res == -1) {
                ret = 2;
                // goto cleanup;
        }
        res = arrange_info(opt, file_count, files, matches, &output,
                           match_count, match_count_ignore);
        if (res == 0 && ret < 1) {
                ret = 1;
        }
        printf("%s", output.data);
cleanup:
        string_destory(&output);
        destroy_lines(matches, file_count);

        return ret;
}