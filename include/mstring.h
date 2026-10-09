#ifndef MSTRING_H
#define MSTRING_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t size;
    size_t capacity;
    char data[];
} m_string;
/*申请一个m_string,申请失败返回空指针
 *传入的cap应在size_t范围之内，超出概不负责
 */
m_string *string_init(size_t cap);
/*压入一个字符，空间不足自动扩容，扩容超出size_t范围不负责
 * 
 * 
 */
int push_back(m_string **str, char c);
void update_size(m_string *str);

void string_destroy(m_string* str);
/*对字符串进行扩容处理
 *应保证cap是大于现有size的，且在size_t范围之内，出错概不负责
 */
void reserve(m_string** str,size_t cap);
int empty(m_string* str);
void concatenate(m_string **dest,char *src);
void clear(m_string* str);
#endif

