#include "parse.h"
#include "constant.h"
#include "error.h"
#include <asm-generic/errno-base.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*
调用者应保证参数均合法
option_receiver容量为MAX_OPTION_COUNT
*/
static int process_option(const char *option_deliver, char *option_receiver) {
        static int count = 0;
        const char legal_options[] = "nic";
        for (int j = 1; option_deliver[j] != '\0'; j++) {
                char opt = option_deliver[j];
                if (strchr(legal_options, opt) == NULL) {
                        strcpy(filename, "all");
                        exit_code = 2;
                        return ERR_ILLEGAL_OPTION;
                }
                if (strchr(option_receiver, opt) == NULL) {
                        option_receiver[count] = opt;
                        count++;
                }
        }
        return SUCCESS;
}
/*
调用者应保证参数均合法
pattern_receiver容量为PATTERN_LENGTH
*/
static int process_pattern(char *pattern_deliver, char *pattern_receiver) {
        size_t len = strlen(pattern_deliver);
        if (len >= PATTERN_LENGTH) {
                strcpy(filename, "all");
                exit_code = 2;
                return ERR_PATTERN_TOO_LONG;
        }
        memcpy(pattern_receiver, pattern_deliver, len + 1);
        return SUCCESS;
}
/*
调用者应保证参数均合法
*/
static int process_file(const char *file_deliver, char *file_receiver) {
        size_t len = strlen(file_deliver);
        if (len >= FILE_NAME_LENGTH) {
                exit_code = 2;
                strcpy(filename, file_deliver);
                return ERR_FILE_NAME_TOO_LONG;
        }
        memcpy(file_receiver, file_deliver, len + 1);
        return SUCCESS;
}

int parse_args(char **argv, int argc, char *options, char *pattern,
               char (*files)[FILE_NAME_LENGTH], int *file_count) {
        if (!argv || !options || !pattern || !files || !file_count) {
                exit_code = 2;
                return ERR_BAD_PARAMETER;
        }

        *file_count = 0;
        bool pattern_exist = false;
        for (int i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (!arg)
                        continue;
                if (arg[0] == '-') {
                        int res = process_option(arg, options);
                        if (res != SUCCESS)
                                return res;
                        continue;
                }
                if (!pattern_exist) {
                        int res = process_pattern(arg, pattern);
                        if (res != SUCCESS)
                                return res;
                        pattern_exist = true;
                } else {
                        if (*file_count >= MAX_FILE_COUNT) {
                                exit_code = 2;
                                return ERR_OUT_OF_MAX_FILE_COUNT;
                        }
                        int res = process_file(arg, files[*file_count]);
                        if (res != SUCCESS)
                                return res;
                        (*file_count)++;
                }
        }
        if (!pattern_exist) {
                strcpy(filename, "all");
                exit_code = 2;
                return ERR_NO_PATTERN;
        }
        if ((*file_count) == 0) {
            strcpy(files[*file_count],"/dev/stdin");
            (*file_count)++;
        }
        return SUCCESS;
}