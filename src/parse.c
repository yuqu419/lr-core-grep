#include "parse.h"
#include "constant.h"
#include "error.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_option(const char *option_deliver, char *option_receiver,size_t* count) {
        const char legal_options[] = "nic";
        for (int j = 1; option_deliver[j] != '\0'; j++) {
                char opt = option_deliver[j];
                if (strchr(legal_options, opt) == NULL) {
                        report_err(0, NULL);
                        return -1;
                }
                if (strchr(option_receiver, opt) == NULL) {
                        option_receiver[*count] = opt;
                        (*count)++;
                }
        }
        return 0;
}

static void process_pattern(const char *pattern_deliver,
                            char *pattern_receiver) {
        size_t len = strlen(pattern_deliver);
        memcpy(pattern_receiver, pattern_deliver, len + 1);
}

static void process_file(const char *file_deliver, char *file_receiver) {
        size_t len = strlen(file_deliver);
        memcpy(file_receiver, file_deliver, len + 1);
}

static void set_option_by_str(const char *opt_str, option *opt) {
        if (strchr(opt_str, 'c')) {
                *opt = COUNT;
                if (strchr(opt_str, 'i')) {
                        *opt = IGNORE_AND_COUNT;
                }
                return;
        }
        if (strchr(opt_str, 'n')) {
                *opt = NUMBER;
                if (strchr(opt_str, 'i')) {
                        *opt = IGNORE_AND_NUMBER;
                }
                return;
        }
        if (strchr(opt_str, 'i')) {
                *opt = IGNORE;
        }
        return;
}

int parse_args(char **argv, int argc, option *opt, char *pattern,
               char (*files)[FILE_NAME_SIZE], size_t *file_count) {
        char opt_str[4] = {0};
        *file_count = 0;
        size_t option_count = 0;
        bool pattern_exist = false;
        for (int i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (!arg)
                        continue;
                if (arg[0] == '-' && arg[1] != '\0' && !pattern_exist) {
                        int res = process_option(arg, opt_str,&option_count);
                        if (res == -1) {
                                return -1;
                        }
                        continue;
                }
                if (!pattern_exist) {
                        process_pattern(arg, pattern);
                        pattern_exist = true;
                } else {
                        process_file(arg, files[*file_count]);
                        (*file_count)++;
                }
        }
        if (!pattern_exist) {
                report_err(0, NULL);
                return -1;
        }
        set_option_by_str(opt_str, opt);
        // 从stdin加载
        if (*file_count == 0) {
                process_file("/dev/stdin", files[*file_count]);
                (*file_count)++;
        }
        return 0;
}