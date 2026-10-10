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
        char *ptr = err_str;
        size_t remaining = strlen(err_str);
        while (remaining > 0) {
                ssize_t n = write(STDERR_FILENO, ptr, remaining);
                if (n == -1) {
                        if (errno == EINTR)
                                continue;
                        break;
                }
                ptr += n;
                remaining -= n;
        }   
}
