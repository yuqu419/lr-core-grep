#include "mstring.h"
#include <string.h>

m_string *string_init(size_t cap) {
        m_string *str = malloc(sizeof(m_string) + cap);
        if (!str)
                return NULL;
        *str = (m_string){.size = 0, .capacity = cap};
        memset(str->data, 0, cap);
        return str;
}
int push_back(m_string **str, char c) {
        size_t size = (*str)->size;
        size_t cap = (*str)->capacity;
        if (size >= cap - 1) {
                m_string *new_ptr =
                    realloc(*str, sizeof(m_string) + cap * 2 + 1);
                if (new_ptr != NULL)
                        (*str) = new_ptr;
                (*str)->capacity = cap * 2 + 1;
        }
        (*str)->data[size] = c;
        (*str)->size++;
        (*str)->data[(*str)->size] = 0;

        return 0;
}
void clear(m_string *str) {
        memset(str->data, 0, str->size);
        str->size = 0;
}
void update_size(m_string *str) {
        if (!str)
                return;
        str->size = strlen(str->data);
}
void reserve(m_string **str, size_t cap) {
        if (cap <= (*str)->capacity)
                return;
        m_string *new_ptr = realloc(*str, sizeof(m_string) + cap);
        if (new_ptr != NULL)
                (*str) = new_ptr;
        (*str)->capacity = cap;
}
void concatenate(m_string **dest, char *src) {
        size_t size = strlen(src);
        reserve(dest, (*dest)->size + size);
        memcpy((*dest)->data + (*dest)->size, src, size);
        (*dest)->size += size;
}
int empty(m_string *str) { return !(str->size); }
void string_destroy(m_string *str) { free(str); }
