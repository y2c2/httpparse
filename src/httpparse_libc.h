/* HTTP Parse : LibC
 * Copyright(c) 2016 y2c2 */

#ifndef HTTPPARSE_LIBC_H
#define HTTPPARSE_LIBC_H

#include "httpparse_dt.h"

char *hp_strdup(const char *s);
void *hp_memcpy(void *dest, const void *src, hp_size_t n);
hp_size_t hp_strlen(const char *s);
void *hp_memset(void *s, int c, hp_size_t n);
int hp_strcmp(const char *s1, const char *s2);
int hp_strncmp(const char *s1, const char *s2, hp_size_t n);
const char *hp_strnstrn( \
        const char *haystack, \
        const hp_size_t haystack_len, \
        const char *needle, \
        const hp_size_t needle_len);

#endif

