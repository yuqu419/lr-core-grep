#ifndef M_STRING_H
#define M_STRING_H


#include <stddef.h>
#include <stdlib.h>

/*
整个m_string存储在栈中，会将data声明在堆中中，init申请，destroy释放
size 代表不包含结尾'\0'的所有字符长度，但字符串严格以'\0'结尾，所以
要求cap > size

*/
typedef struct m_string {
		char *data;
		size_t size;
		size_t cap;
} m_string;
/*
调用者应保证str非空
当cap为0时，会直接返回0
初始化字符串，为data申请空间，
将所有字符初始化为0
成功返回0，失败返回-1
失败后会设定errno

*/

int string_init(m_string *str, size_t cap);
/*
调用者应保证str非空
为str预留cap个空间，不改变现有数据和size
若cap小于等于cap，则直接返回

*/
int string_reserve(m_string *str, size_t cap);

/*
调用者应保证dest和src非空
将src的len个字符严格拼接至dest之后，dest的末尾零会被覆盖
拼接后的字符串保证以'\0'结尾

*/

int string_concat(m_string *dest, const char *src, size_t len);

/*
调用者保证str非空，
若size = 0,返回1，反则返回1
如果返回负数，说明出错了
*/
int string_empty(m_string *str);
/*
调用者保证str非空，
将全部data置为0
size置为0，cap大小不变
*/

int string_clear(m_string *str);

/*
调用者保证str非空，
释放data,并置空，size和cap置为0,成功返回0，失败返回-1

*/
int string_destroy(m_string *str);
#endif