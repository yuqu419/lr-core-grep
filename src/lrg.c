#include "arrange.h"
#include "constant.h"
#include "mstring.h"
#include "parse.h"
#include "search.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
static void match_destroy(match_line *matches[MAX_FILE_COUNT],
                          size_t match_cap[MAX_FILE_COUNT]) {
        for (size_t i = 0; i < MAX_FILE_COUNT; i++) {
                if (match_cap[i] == 0)
                        continue;
                for (size_t j = 0; j < match_cap[i]; j++) {
                        string_destroy(&matches[i][j].content);
                }
                free(matches[i]);
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
        match_line *matches[MAX_FILE_COUNT] = {0};
        size_t match_cap[MAX_FILE_COUNT] = {0};
        size_t match_count[MAX_FILE_COUNT] = {0};
        size_t match_count_ignore[MAX_FILE_COUNT] = {0};
        m_string output;
        string_init(&output, BUFFER_SIZE);
        res = collect_all(files, file_count, pattern, matches, match_count,
                          match_count_ignore, match_cap);
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
        match_destroy(matches, match_cap);
        string_destroy(&output);

        return ret;
}