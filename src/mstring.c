#include "mstring.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


int string_init(m_string *str, size_t cap) {
		if (cap == 0) {
				*str = (m_string){.size = 0, .cap = 0, .data = NULL};
				return 0;
		}
		void *new_ptr = calloc(cap, 1);
		if (!new_ptr)
				return -1;
		*str = (m_string){.cap = cap, .data = (char *)new_ptr, .size = 0};
		return 0;
}

int string_reserve(m_string *str, size_t cap) {
		if (str->cap >= cap)
				return 0;
		void *new_ptr = realloc(str->data, cap);
		if (!new_ptr)
				return -1;
		memset(new_ptr + str->cap, 0, cap - str->cap);
		*str = (m_string){.data = new_ptr, .size = str->size, .cap = cap};
		return 0;
}


int string_concat(m_string *dest, const char *src, size_t len) {
		size_t new_cap = (dest->size) + len + 1;
		int res = string_reserve(dest, new_cap);
		if (res == -1)
				return -1;
		memcpy(dest->data + dest->size, src, len);
		*dest = (m_string){
			.data = dest->data, .cap = dest->cap, .size = dest->size + len};
		return 0;
}


int string_clear(m_string *str) {
		memset(str->data, 0, str->size);
		str->size = 0;
		return 0;
}

int string_empty(m_string *str) {
    return !(str ->size);
}

int string_destroy(m_string *str) {
		free(str->data);
		*str = (m_string){.data = NULL, .cap = 0, .size = 0};
		return 0;
}