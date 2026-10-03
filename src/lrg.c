#include "error.h"
#include "mstring.h"
#include "parse.h"
#include "search.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
        int ret = SUCCESS;
        char pattern[PATTERN_LENGTH] = {0};
        char options[MAX_OPTION_COUNT] = {0};
        char files[MAX_FILE_COUNT][FILE_NAME_LENGTH] = {0};
        int file_count = 0;
        bool no_match = true;
        m_string *match = string_init(BUFFER_SIZE);
        ret = parse_args(argv, argc, options, pattern, files, &file_count);
        if (ret != SUCCESS) {
                goto cleanup;
        }
        for (int i = 0; i < file_count; i++) {

                char *file = files[i];
                if (file_count > 1) {
                        printf("%s\n", file);
                }
                ret = collect_lines(file, pattern, options, match, &no_match);
                if (ret != SUCCESS) {
                        goto cleanup;
                }
                printf("%s", match->data);
                clear(match);
        }
        if (no_match)
                exit_code = 1;
cleanup:
        if (ret != SUCCESS) {
                ret = handle_error(ret);
        }
        string_destroy(match);
        return exit_code;
}
