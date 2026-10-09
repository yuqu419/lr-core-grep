#include <stdio.h>
#include <string.h>

#include "error.h"
#include "mstring.h"
#include "parse.h"
#include "search.h"





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
        bool countable = strchr(options, 'c') != NULL;
        for (int i = 0; i < file_count; i++) {
                char *file = files[i];
                ret = collect_lines(file, pattern, options, &match, &no_match);
                if (ret != SUCCESS) {
                        if (ret == ERR_FILE_READ || ret == ERR_FILE_OPEN) {
                                handle_error(ret);
                                ret = SUCCESS;
                                exit_code = 2;
                                continue;
                        }
                        goto cleanup;
                }
                if (file_count > 1) {
                        printf("%s:", file);
                }
                printf("%s", match->data);
                clear(match);
        }
        if (no_match && exit_code == 0) {
                if (countable)
                        printf("0");
                exit_code = 1;
        }
cleanup:
        if (ret != SUCCESS) {
                ret = handle_error(ret);
        }
        string_destroy(match);
        return exit_code;
}
