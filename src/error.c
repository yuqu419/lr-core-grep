// #include "error.h"
// #include "constant.h"
// #include <errno.h>
// #include <fcntl.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/types.h>
// #include <unistd.h>
// int exit_code = 0;
// char filename[4096 * 2] = {0};
// int get_str_by_error(int err, char *err_info) {
//         switch (err) {
//         case 0:
//                 strcpy(err_info, "No error");
//                 break;
//         case -1:
//                 strcpy(err_info, "Every pointer should not be NULL");
//                 break;
//         case -2:
//                 strcpy(err_info, "The number of files should not beyond
//                 256"); break;
//         case -3:
//                 strcpy(err_info, "Please enter legal pattern");
//                 break;
//         case -4:
//                 strcpy(err_info, "The options only include n,i and c");
//                 break;
//         case -5:
//                 strcpy(err_info, "The length of a file should not beyond
//                 256"); break;
//         case -6:
//                 strcpy(err_info,
//                        "The length of pattern should not beyond 1024");
//                 break;
//         default:
//                 return -1;
//                 break;
//         }
//         return 0;
// }
// int handle_error(int err) {
//         char err_str[MAX_ERROR_DESCRIPTION_LENGTH] = {0};
//         if (err == -7 || err == -8) {
//                 char *tmp = strerror(errno);
//                 strcpy(err_str, tmp);
//         }
//         int res = get_str_by_error(err, err_str);
//         int total = strlen(err_str) + 4;
//         char tmp[MAX_ERROR_DESCRIPTION_LENGTH] = {0};
//         strcpy(tmp, err_str);
//         memset(err_str, 0, MAX_ERROR_DESCRIPTION_LENGTH);
//         strcpy(err_str, "lrg:");
//         strcat(strcat(err_str, filename), ":");
//         strcat(strcat(err_str, tmp), "\n");
//         while (true) {
//                 ssize_t write_byte = write(STDERR_FILENO, err_str, total +
//                 1); total -= write_byte; if (total <= 0) {
//                         break;
//                 }
//         }
//         return res;
// }

/*
接收错误码,判断错误类型
1.用法错误:输出指定文本
2.系统错误:依据指定格式打印系统错误
3.致命错误:程序直接退出
4.可追溯错误:程序继续运行
*/
// 0为用法错误
// 1为系统错误
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
void report_err(int err, char *filename) {
        char err_str[4096] = {0};

        if (err == 0) {
                strcpy(err_str, "usage: lrg [-nic] PATTERN [FILE...]\n");

        } else if (err == 1) {
                strcat(err_str, "lrg: ");
                strcat(err_str, filename);
                strcat(err_str, ": ");
                strcat(err_str, strerror(errno));
                strcat(err_str, "\n");
        }
        size_t total = strlen(err_str);
        while (true) {
                ssize_t write_byte =
                    write(STDERR_FILENO, err_str, total);
                if (write_byte == -1) {
                        if (errno == EINTR)
                                continue;
                        break;
                }
                if (total <= 0)
                        break;
                total -= write_byte;
        }
}
