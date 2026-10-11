#include "arrange.h"
#include "constant.h"
#include "mstring.h"
#include "parse.h"
#include "search.h"
#include <stddef.h>
#include <stdio.h>
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
                        string_destroy(&(matches[i][j].content));
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
        }
        res = arrange_info(opt, file_count, files, matches, &output,
                           match_count, match_count_ignore);
        if (res == 0 && ret < 1) {
                ret = 1;
        }
        printf("%s", output.data);
cleanup:
        string_destroy(&output);
        destroy_lines(matches, file_count);

        return ret;
}