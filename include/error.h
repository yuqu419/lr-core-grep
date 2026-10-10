#ifndef  ERROR_H
#define  ERROR_H
// #include <limits.h>
// typedef enum {
//         SUCCESS = 0,
//         ERR_BAD_PARAMETER = -1,
//         ERR_OUT_OF_MAX_FILE_COUNT = -2,
//         ERR_NO_PATTERN = -3,
//         ERR_ILLEGAL_OPTION = -4,
//         ERR_FILE_NAME_TOO_LONG = -5,
//         ERR_PATTERN_TOO_LONG = -6,
//         ERR_FILE_OPEN = -7,
//         ERR_FILE_READ = -8
// } error;
// int handle_error(int err);
// extern int exit_code;
// extern char filename[4096 * 2];
void report_err(int err, char *filename);
#endif